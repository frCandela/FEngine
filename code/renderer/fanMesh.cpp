#include "fanGlobalIncludes.h"

#include "renderer/fanMesh.h"
#include "core/files/fanFbxImporter.h"
#include "core/math/shapes/fanConvexHull.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanRendererDebug.h"

namespace fan {
	REGISTER_TYPE_INFO(Mesh, TypeInfo::Flags::NONE, "scene/")

	Signal< Mesh* > Mesh::s_onGenerateVulkanData;
	Signal< Mesh* > Mesh::s_onDeleteVulkanData;

	//================================================================================================================================
	//================================================================================================================================
	Mesh::Mesh( ) 
		: m_vertices( 0 )
		, m_indices( 0 )
		, m_vertexBuffer { nullptr ,nullptr ,nullptr }
		, m_indexBuffer { nullptr ,nullptr ,nullptr }
	{}

	//================================================================================================================================
	//================================================================================================================================
	Mesh::~Mesh() { s_onDeleteVulkanData.Emmit( this ); }

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh::LoadFromFile( const std::string& _path ) {
 		FBXImporter importer;
 		if (importer.LoadScene(_path) == true) {
			if ( ! importer.GetMesh(*this) ) {
				Debug::Get() << "Failed to load mesh : " << m_path << Debug::Endl();
				return false;
			}	
		}		
		OptimizeVertices();
		GenerateConvexHull();
		s_onGenerateVulkanData.Emmit( this );
		return Ressource::LoadFromFile(_path);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh::LoadFromVertices( const std::vector<Vertex>&	_vertices ) {
		m_vertices = _vertices;

		// Generate fake indices
		m_indices.clear();
		m_indices.reserve(_vertices.size());
		for (int vertIndex = 0; vertIndex < _vertices.size(); vertIndex++) {
			m_indices.push_back( vertIndex );
		}

		// Cleanup
		OptimizeVertices();
		GenerateConvexHull();
		s_onGenerateVulkanData.Emmit( this );
		return true;
	}

	
	//================================================================================================================================
	// Removes duplicates vertices & generates a corresponding index buffer
	//================================================================================================================================
	void Mesh::OptimizeVertices() {

		if( !m_optimizeVertices ) { return;}

		std::unordered_map<Vertex, uint32_t> verticesMap = {};

		std::vector<Vertex> uniqueVertices;
		std::vector<uint32_t>	uniqueIndices;

		for (int indexIndex = 0; indexIndex < m_indices.size(); indexIndex++) {
			Vertex vertex = m_vertices[m_indices[indexIndex]];
			auto it = verticesMap.find(vertex);
			if (it == verticesMap.end()) {
				verticesMap[vertex] = static_cast<uint32_t>(uniqueVertices.size());
				uniqueIndices.push_back(static_cast<uint32_t>(uniqueVertices.size()));
				uniqueVertices.push_back(vertex);				
			} else {
				uniqueIndices.push_back(it->second);
			}
		}
		m_vertices = uniqueVertices;
		m_indices = uniqueIndices;
	} 

	//================================================================================================================================
	// Creates a convex hull from the mesh geometry
	//================================================================================================================================
	void  Mesh::GenerateConvexHull( ) {
		if( m_vertices.empty() || ! m_autoUpdateHull ) { return; }

		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve(m_vertices.size());
		for (int point = 0; point < m_vertices.size(); point++) {
			Vertex & vertex = m_vertices[point];
			pointCloud.push_back(btVector3(vertex.pos.x, vertex.pos.y, vertex.pos.z));
		}
		m_convexHull.ComputeQuickHull(pointCloud);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::GenerateVulkanData( Device & _device ) {
		if ( m_indices.empty() ){return;}

		m_currentBuffer = ( m_currentBuffer + 1 ) % 3;

		const VkMemoryPropertyFlags memPropertyFlags = ( m_hostVisible ? 	
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		Buffer *& vertexBuffer = m_vertexBuffer[m_currentBuffer];
		const VkDeviceSize requiredVertexSize = sizeof( m_vertices[0] ) * m_vertices.size();
		if ( vertexBuffer == nullptr || vertexBuffer->GetSize() < requiredVertexSize ){
			delete vertexBuffer;
			vertexBuffer = new Buffer( _device );
			m_vertexBuffer[m_currentBuffer]->Create(
				requiredVertexSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				memPropertyFlags
			);
		}

		Buffer *& indexBuffer = m_indexBuffer[m_currentBuffer];
		const VkDeviceSize requiredIndexSize = sizeof( m_indices[0] ) * m_indices.size();
		if ( indexBuffer == nullptr || indexBuffer->GetSize() < requiredIndexSize )
		{
			delete indexBuffer;
			indexBuffer = new Buffer( _device );
			indexBuffer->Create(
				requiredIndexSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				memPropertyFlags
			);
		}

		if ( m_hostVisible )
		{
			indexBuffer->SetData( m_indices.data(), requiredIndexSize );
			vertexBuffer->SetData( m_vertices.data(), requiredVertexSize );
		}
		else {
			{
				Buffer stagingBuffer( _device );
				stagingBuffer.Create(
					requiredIndexSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( m_indices.data(), requiredIndexSize );
				VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, indexBuffer->GetBuffer(), requiredIndexSize );
				_device.EndSingleTimeCommands( cmd );
			}
			{


				Buffer stagingBuffer2( _device );
				stagingBuffer2.Create(
					requiredVertexSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer2.SetData( m_vertices.data(), requiredVertexSize );
				VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
				stagingBuffer2.CopyBufferTo( cmd2, vertexBuffer->GetBuffer(), requiredVertexSize );
				_device.EndSingleTimeCommands( cmd2 );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::DeleteVulkanData( Device & /*_device*/ )
	{
		for ( int bufferIndex = 0; bufferIndex < 3; bufferIndex++ )
		{
			delete m_indexBuffer[bufferIndex];
			delete m_vertexBuffer[bufferIndex];
		}
	}

	//================================================================================================================================
	// Raycast on all triangles of the convex hull
	//================================================================================================================================
	bool Mesh::RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const
	{
		btVector3 intersection;
		float closestDistance = std::numeric_limits<float>::max();
		for ( int triIndex = 0; triIndex < m_indices.size() / 3; triIndex++ )
		{
			const btVector3 v0 = ToBullet( m_vertices[m_indices[3 * triIndex + 0]].pos );
			const btVector3 v1 = ToBullet( m_vertices[m_indices[3 * triIndex + 1]].pos );
			const btVector3 v2 = ToBullet( m_vertices[m_indices[3 * triIndex + 2]].pos );
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