#include "fanMesh.hpp"
#include "core/fanPath.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"
#include "core/shapes/fanTriangle.hpp"
#include "core/shapes/fanAABB.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Mesh::LoadFromFile( const std::string& _path )
    {
        if( _path.empty() )
        {
            Debug::Warning() << "Trying to load mesh with an empty path" << Debug::Endl();
            return false;
        }

        GLTFImporter importer;
        if( importer.Load( Path::Normalize( _path ) ) )
        {
            importer.GetMesh( *this );
            for( SubMesh& subMesh : mSubMeshes )
            {
                subMesh.OptimizeVertices();
            }
            GenerateBoundingVolumes();

            mPath = _path;
            return true;
        }

        Debug::Warning() << "Failed to load mesh : " << mPath << Debug::Endl();
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool SubMesh::LoadFromVertices()
    {
        // Generate fake indices
        mIndices.clear();
        mIndices.reserve( mVertices.size() );
        for( int vertIndex = 0; vertIndex < (int)mVertices.size(); vertIndex++ )
        {
            mIndices.push_back( vertIndex );
        }

        // Cleanup
        OptimizeVertices();

        return true;
    }

    //==================================================================================================================================================================================================
    // Removes duplicates vertices & generates a corresponding index buffer
    //==================================================================================================================================================================================================
    void SubMesh::OptimizeVertices()
    {

        if( !mOptimizeVertices ){ return; }

        std::unordered_map<Vertex, uint32_t> verticesMap = {};

        std::vector<Vertex>   uniqueVertices;
        std::vector<uint32_t> uniqueIndices;

        for( int indexIndex = 0; indexIndex < (int)mIndices.size(); indexIndex++ )
        {
            Vertex vertex = mVertices[mIndices[indexIndex]];
            auto   it     = verticesMap.find( vertex );
            if( it == verticesMap.end() )
            {
                verticesMap[vertex] = static_cast< uint32_t >( uniqueVertices.size() );
                uniqueIndices.push_back( static_cast< uint32_t >( uniqueVertices.size() ) );
                uniqueVertices.push_back( vertex );
            }
            else
            {
                uniqueIndices.push_back( it->second );
            }
        }
        mVertices = uniqueVertices;
        mIndices  = uniqueIndices;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Mesh::Empty() const
    {
        for( SubMesh subMesh : mSubMeshes )
        {
            if( !subMesh.mIndices.empty() )
            {
                return false;
            }
        }
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    int Mesh::CountVertices() const
    {
        int numVertices = 0;
        for( const SubMesh& subMesh : mSubMeshes )
        {
            numVertices += (int)subMesh.mVertices.size();
        }
        return numVertices;
    }

    //==================================================================================================================================================================================================
    // Creates a convex hull from the mesh geometry
    //==================================================================================================================================================================================================
    void Mesh::GenerateBoundingVolumes()
    {
        if( !mAutoGenerateBoundingVolumes ){ return; }

        // Generate points clouds from vertex list
        std::vector<Vector3> pointCloud;
        pointCloud.reserve( CountVertices() );
        for( SubMesh& subMesh : mSubMeshes )
        {
            for( int point = 0; point < (int)subMesh.mVertices.size(); point++ )
            {
                Vertex& vertex = subMesh.mVertices[point];
                pointCloud.push_back( Vector3( vertex.mPos ) );
            }
        }

        if( !pointCloud.empty() )
        {
            mConvexHull.ComputeQuickHull( pointCloud );
        }

        // compute bounding sphere
        const AABB aabb( mConvexHull.mVertices );
        mBoundingSphere.mCenter = aabb.GetCenter();
        mBoundingSphere.mRadius = 0;
        for( Vector3 point : mConvexHull.mVertices )
        {
            Fixed sqrDistance = Vector3::SqrDistance( point, mBoundingSphere.mCenter );
            if( sqrDistance > mBoundingSphere.mRadius )
            {
                mBoundingSphere.mRadius = sqrDistance;
            }
        }
        mBoundingSphere.mRadius = Fixed::Sqrt( mBoundingSphere.mRadius );
    }

    //==================================================================================================================================================================================================
    // Raycast on all triangles of the mesh
    //==================================================================================================================================================================================================
    bool SubMesh::RayCast( const Ray _ray, RaycastResult& _outResult ) const
    {
        RaycastResult result;
        Fixed         closestDistance = Fixed::sMaxValue;
        for( int      triIndex        = 0; triIndex < (int)mIndices.size() / 3; triIndex++ )
        {
            const Vector3  v0 = Vector3( mVertices[mIndices[3 * triIndex + 0]].mPos );
            const Vector3  v1 = Vector3( mVertices[mIndices[3 * triIndex + 1]].mPos );
            const Vector3  v2 = Vector3( mVertices[mIndices[3 * triIndex + 2]].mPos );
            const Triangle triangle( v0, v1, v2 );

            if( triangle.RayCast( _ray, result ) )
            {
                if( result.mDistance < closestDistance )
                {
                    closestDistance = result.mDistance;
                    _outResult      = result;
                }
            }
        }
        return closestDistance != Fixed::sMaxValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Mesh::RayCast( const Ray _ray, RaycastResult& _outResult ) const
    {
        for( const SubMesh& subMesh : mSubMeshes )
        {
            if( subMesh.RayCast( _ray, _outResult ) )
            {
                return true;
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SubMesh::Destroy( Device& _device )
    {
        for( int i = 0; i < (int)SwapChain::sMaxFramesInFlight; i++ )
        {
            mIndexBuffer[i].Destroy( _device );
            mVertexBuffer[i].Destroy( _device );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SubMesh::Create( Device& _device )
    {
        if( mIndices.empty() || mVertices.empty() ){ return; }

        mCurrentBuffer = ( mCurrentBuffer + 1 ) % SwapChain::sMaxFramesInFlight;

        const VkMemoryPropertyFlags memPropertyFlags = ( mHostVisible ?
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        Buffer& vertexBuffer = mVertexBuffer[mCurrentBuffer];
        const VkDeviceSize requiredVertexSize = sizeof( mVertices[0] ) * mVertices.size();
        if( vertexBuffer.mBuffer == VK_NULL_HANDLE || vertexBuffer.mSize < requiredVertexSize )
        {
            vertexBuffer.Destroy( _device );
            mVertexBuffer[mCurrentBuffer].Create(
                    _device,
                    requiredVertexSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    memPropertyFlags
            );
            _device.AddDebugName( (uint64_t)vertexBuffer.mBuffer, "mesh vertex buffer" );
            _device.AddDebugName( (uint64_t)vertexBuffer.mMemory, "mesh vertex buffer" );
        }

        Buffer& indexBuffer = mIndexBuffer[mCurrentBuffer];
        const VkDeviceSize requiredIndexSize = sizeof( mIndices[0] ) * mIndices.size();
        if( indexBuffer.mBuffer == VK_NULL_HANDLE || indexBuffer.mSize < requiredIndexSize )
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

        if( mHostVisible )
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