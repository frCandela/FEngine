#include "fanMesh.hpp"
#include "core/fanPath.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/shapes/fanTriangle.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	bool Mesh::LoadFromFile( const std::string& _path )
	{
        if( _path.empty())
        {
            Debug::Warning() << "Trying to load mesh with an empty path" << Debug::Endl();
            return false;
        }

		GLTFImporter importer;
		if ( importer.Load( Path::Normalize( _path ) ) )
		{
			if ( !importer.GetMesh( *this ) )
			{
				Debug::Get() << "Failed to load mesh : " << mPath << Debug::Endl();
				return false;
			}

			OptimizeVertices();
			GenerateConvexHull();
            mBuffersOutdated = true;

            mPath = _path;
			return true;
		}

		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Mesh::LoadFromVertices()
	{
		// Generate fake indices
        mIndices.clear();
        mIndices.reserve( mVertices.size() );
		for ( int vertIndex = 0; vertIndex < (int)mVertices.size(); vertIndex++ )
		{
            mIndices.push_back( vertIndex );
		}

		// Cleanup
		OptimizeVertices();
		GenerateConvexHull();
        mBuffersOutdated = true;

		return true;
	}


	//========================================================================================================
	// Removes duplicates vertices & generates a corresponding index buffer
	//========================================================================================================
	void Mesh::OptimizeVertices()
	{

		if ( !mOptimizeVertices ) { return; }

		std::unordered_map<Vertex, uint32_t> verticesMap = {};

		std::vector<Vertex> uniqueVertices;
		std::vector<uint32_t>	uniqueIndices;

		for ( int indexIndex = 0; indexIndex < (int)mIndices.size(); indexIndex++ )
		{
			Vertex vertex = mVertices[ mIndices[ indexIndex ] ];
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
        mVertices = uniqueVertices;
        mIndices = uniqueIndices;
	}

	//================================================================================================================================
	// Creates a convex hull from the mesh geometry
	//========================================================================================================
	void  Mesh::GenerateConvexHull()
	{
		if ( mVertices.empty() || !mAutoUpdateHull ) { return; }

		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve( mVertices.size() );
		for ( int point = 0; point < (int)mVertices.size(); point++ )
		{
			Vertex& vertex = mVertices[ point ];
			pointCloud.push_back( btVector3( vertex.mPos.x, vertex.mPos.y, vertex.mPos.z ) );
		}
		mConvexHull.ComputeQuickHull( pointCloud );
	}

	//========================================================================================================
	// Raycast on all triangles of the convex hull
	//========================================================================================================
    bool Mesh::RayCast( const btVector3 _origin,
                        const btVector3 _direction,
                        btVector3& _outIntersection ) const
	{
		btVector3 intersection;
		float closestDistance = std::numeric_limits<float>::max();
		for ( int triIndex = 0; triIndex < (int)mIndices.size() / 3; triIndex++ )
		{
			const btVector3 v0 = ToBullet( mVertices[ mIndices[ 3 * triIndex + 0 ] ].mPos );
			const btVector3 v1 = ToBullet( mVertices[ mIndices[ 3 * triIndex + 1 ] ].mPos );
			const btVector3 v2 = ToBullet( mVertices[ mIndices[ 3 * triIndex + 2 ] ].mPos );
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

    //========================================================================================================
    //========================================================================================================
    void Mesh::Destroy( Device & _device )
    {
        for( int i = 0 ; i < (int)SwapChain::sMaxFramesInFlight; i++)
        {
            mIndexBuffer[i].Destroy( _device );
            mVertexBuffer[i].Destroy( _device );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void Mesh::Create( Device& _device )
    {
	    mBuffersOutdated = false;

        if ( mIndices.empty() || mVertices.empty() ) { return; }

        mCurrentBuffer = ( mCurrentBuffer + 1 ) % SwapChain::sMaxFramesInFlight;

        const VkMemoryPropertyFlags memPropertyFlags = ( mHostVisible ?
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        Buffer& vertexBuffer = mVertexBuffer[ mCurrentBuffer ];
        const VkDeviceSize requiredVertexSize = sizeof( mVertices[ 0 ] ) * mVertices.size();
        if ( vertexBuffer.mBuffer == VK_NULL_HANDLE || vertexBuffer.mSize < requiredVertexSize )
        {
            vertexBuffer.Destroy( _device );
            mVertexBuffer[ mCurrentBuffer ].Create(
                    _device,
                    requiredVertexSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    memPropertyFlags
            );
            _device.AddDebugName( (uint64_t)vertexBuffer.mBuffer, "mesh vertex buffer" );
            _device.AddDebugName( (uint64_t)vertexBuffer.mMemory, "mesh vertex buffer" );
        }

        Buffer& indexBuffer = mIndexBuffer[ mCurrentBuffer ];
        const VkDeviceSize requiredIndexSize = sizeof( mIndices[ 0 ] ) * mIndices.size();
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

        if ( mHostVisible )
        {
            indexBuffer.SetData( _device, mIndices.data(), requiredIndexSize );
            vertexBuffer.SetData( _device, mVertices.data(), requiredVertexSize );
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
                stagingBuffer.SetData( _device, mIndices.data(), requiredIndexSize );
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
                stagingBuffer2.SetData( _device, mVertices.data(), requiredVertexSize );
                VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
                stagingBuffer2.CopyBufferTo( cmd2, vertexBuffer.mBuffer, requiredVertexSize );
                _device.EndSingleTimeCommands( cmd2 );
                stagingBuffer2.Destroy( _device );
            }
        }
    }
}