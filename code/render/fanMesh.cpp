#include "render/fanMesh.hpp"

#include "render/fanGLTFImporter.hpp"
#include "render/fanMeshManager.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/shapes/fanConvexHull.hpp"
#include "core/shapes/fanTriangle.hpp"

namespace fan
{
	MeshManager	    Mesh::s_resourceManager;

	//================================================================================================================================
	//================================================================================================================================
	Mesh::Mesh()
		: m_vertices( 0 )
		, m_indices( 0 )
	{}

	//================================================================================================================================
	//================================================================================================================================
	Mesh::~Mesh() 
	{ 
		if( !s_resourceManager.IsCleared() ) // @hack
		{
			DeleteGpuBuffers( s_resourceManager.GetDevice() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh::LoadFromFile( const std::string& _path, bool _genBuffers )
	{
		GLTFImporter importer;
		if ( importer.Load( _path ) )
		{
			if ( !importer.GetMesh( *this ) )
			{
				Debug::Get() << "Failed to load mesh : " << mPath << Debug::Endl();
				return false;
			}

			OptimizeVertices();
			GenerateConvexHull();
			if( _genBuffers )GenerateGpuBuffers( s_resourceManager.GetDevice() );

            mPath = _path;
			return true;
		}

		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh::LoadFromVertices( const std::vector<Vertex>& _vertices )
	{
		m_vertices = _vertices;

		// Generate fake indices
		m_indices.clear();
		m_indices.reserve( _vertices.size() );
		for ( int vertIndex = 0; vertIndex < (int)_vertices.size(); vertIndex++ )
		{
			m_indices.push_back( vertIndex );
		}

		// Cleanup
		OptimizeVertices();
		GenerateConvexHull();
		GenerateGpuBuffers( s_resourceManager.GetDevice() );
		return true;
	}


	//================================================================================================================================
	// Removes duplicates vertices & generates a corresponding index buffer
	//================================================================================================================================
	void Mesh::OptimizeVertices()
	{

		if ( !m_optimizeVertices ) { return; }

		std::unordered_map<Vertex, uint32_t> verticesMap = {};

		std::vector<Vertex> uniqueVertices;
		std::vector<uint32_t>	uniqueIndices;

		for ( int indexIndex = 0; indexIndex < (int)m_indices.size(); indexIndex++ )
		{
			Vertex vertex = m_vertices[ m_indices[ indexIndex ] ];
			auto it = verticesMap.find( vertex );
			if ( it == verticesMap.end() )
			{
				verticesMap[ vertex ] = static_cast< uint32_t >( uniqueVertices.size() );
				uniqueIndices.push_back( static_cast< uint32_t >( uniqueVertices.size() ) );
				uniqueVertices.push_back( vertex );
			}
			else
			{
				uniqueIndices.push_back( it->second );
			}
		}
		m_vertices = uniqueVertices;
		m_indices = uniqueIndices;
	}

	//================================================================================================================================
	// Creates a convex hull from the mesh geometry
	//================================================================================================================================
	void  Mesh::GenerateConvexHull()
	{
		if ( m_vertices.empty() || !m_autoUpdateHull ) { return; }

		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve( m_vertices.size() );
		for ( int point = 0; point < (int)m_vertices.size(); point++ )
		{
			Vertex& vertex = m_vertices[ point ];
			pointCloud.push_back( btVector3( vertex.pos.x, vertex.pos.y, vertex.pos.z ) );
		}
		m_convexHull.ComputeQuickHull( pointCloud );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::GenerateGpuBuffers( Device& _device ) {
		if ( m_indices.empty() ) { return; }

		m_currentBuffer = ( m_currentBuffer + 1 ) % 3;

		const VkMemoryPropertyFlags memPropertyFlags = ( m_hostVisible ?
														 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
														 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		Buffer& vertexBuffer = m_vertexBuffer[ m_currentBuffer ];
		const VkDeviceSize requiredVertexSize = sizeof( m_vertices[ 0 ] ) * m_vertices.size();
		if ( vertexBuffer.mBuffer == VK_NULL_HANDLE || vertexBuffer.mSize < requiredVertexSize )
		{
			vertexBuffer.Destroy( _device );
			m_vertexBuffer[ m_currentBuffer ].Create( _device,
				requiredVertexSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				memPropertyFlags
			);
			_device.AddDebugName( (uint64_t)vertexBuffer.mBuffer, "mesh vertex buffer" );
			_device.AddDebugName( (uint64_t)vertexBuffer.mMemory, "mesh vertex buffer" );
		}

		Buffer& indexBuffer = m_indexBuffer[ m_currentBuffer ];
		const VkDeviceSize requiredIndexSize = sizeof( m_indices[ 0 ] ) * m_indices.size();
		if ( indexBuffer.mBuffer == VK_NULL_HANDLE || indexBuffer.mSize < requiredIndexSize )
		{
			indexBuffer.Destroy( _device );
			indexBuffer.Create( _device,
				requiredIndexSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				memPropertyFlags
			);
			_device.AddDebugName( (uint64_t)indexBuffer.mBuffer, "mesh index buffer" );
			_device.AddDebugName( (uint64_t)indexBuffer.mMemory, "mesh index buffer" );
		}

		if ( m_hostVisible )
		{
			indexBuffer.SetData( _device, m_indices.data(), requiredIndexSize );
			vertexBuffer.SetData( _device, m_vertices.data(), requiredVertexSize );
		}
		else
		{
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					_device,
					requiredIndexSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _device, m_indices.data(), requiredIndexSize );
				VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, indexBuffer.mBuffer, requiredIndexSize );
				_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( _device );
			}
			{
				Buffer stagingBuffer2;
				stagingBuffer2.Create(
					_device,
					requiredVertexSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer2.SetData( _device, m_vertices.data(), requiredVertexSize );
				VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
				stagingBuffer2.CopyBufferTo( cmd2, vertexBuffer.mBuffer, requiredVertexSize );
				_device.EndSingleTimeCommands( cmd2 );
				stagingBuffer2.Destroy( _device );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::DeleteGpuBuffers( Device& _device ) 
	{
		Debug::Highlight( "Renderer idle2" );
		vkDeviceWaitIdle( _device.mDevice );

		for ( int bufferIndex = 0; bufferIndex < 3; bufferIndex++ )
		{
			m_indexBuffer[bufferIndex].Destroy( _device );
			m_vertexBuffer[bufferIndex].Destroy( _device );
		}
	}

	//================================================================================================================================
	// Raycast on all triangles of the convex hull
	//================================================================================================================================
	bool Mesh::RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const
	{
		btVector3 intersection;
		float closestDistance = std::numeric_limits<float>::max();
		for ( int triIndex = 0; triIndex < (int)m_indices.size() / 3; triIndex++ )
		{
			const btVector3 v0 = ToBullet( m_vertices[ m_indices[ 3 * triIndex + 0 ] ].pos );
			const btVector3 v1 = ToBullet( m_vertices[ m_indices[ 3 * triIndex + 1 ] ].pos );
			const btVector3 v2 = ToBullet( m_vertices[ m_indices[ 3 * triIndex + 2 ] ].pos );
			const Triangle triangle( v0, v1, v2 );

			if ( triangle.RayCast( _origin, _direction, intersection ) )
			{
				float distance = intersection.distance( _origin );
				if ( distance < closestDistance )
				{
					closestDistance = distance;
					_outIntersection = intersection;
				}
			}
		}
		return closestDistance != std::numeric_limits<float>::max();
	}
}