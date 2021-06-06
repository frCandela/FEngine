#include "render/resources/fanMeshSkinned.hpp"
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
    bool MeshSkinned::LoadFromFile( const std::string& _path )
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
            for( SubMeshSkinned& subMesh : mSubMeshes )
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
    bool SubMeshSkinned::LoadFromVertices()
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
    void SubMeshSkinned::OptimizeVertices()
    {

        if( !mOptimizeVertices ){ return; }

        std::unordered_map<VertexSkinned, uint32_t> verticesMap = {};

        std::vector<VertexSkinned> uniqueVertices;
        std::vector<uint32_t>      uniqueIndices;

        for( int indexIndex = 0; indexIndex < (int)mIndices.size(); indexIndex++ )
        {
            VertexSkinned vertex = mVertices[mIndices[indexIndex]];
            auto          it     = verticesMap.find( vertex );
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
    bool MeshSkinned::Empty() const
    {
        for( SubMeshSkinned subMesh : mSubMeshes )
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
    int MeshSkinned::CountVertices() const
    {
        int numVertices = 0;
        for( const SubMeshSkinned& subMesh : mSubMeshes )
        {
            numVertices += (int)subMesh.mVertices.size();
        }
        return numVertices;
    }

    //==================================================================================================================================================================================================
    // Creates a convex hull from the mesh geometry
    //==================================================================================================================================================================================================
    void MeshSkinned::GenerateBoundingVolumes()
    {
        if( !mAutoGenerateBoundingVolumes ){ return; }

        // Generate points clouds from vertex list
        std::vector<Vector3> pointCloud;
        pointCloud.reserve( CountVertices() );
        for( SubMeshSkinned& subMesh : mSubMeshes )
        {
            for( int point = 0; point < (int)subMesh.mVertices.size(); point++ )
            {
                VertexSkinned& vertex = subMesh.mVertices[point];
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
    bool SubMeshSkinned::RayCast( const Ray _ray, RaycastResult& _outResult ) const
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
    bool MeshSkinned::RayCast( const Ray _ray, RaycastResult& _outResult ) const
    {
        for( const SubMeshSkinned& subMesh : mSubMeshes )
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
    void SubMeshSkinned::Destroy( Device& _device )
    {
        mIndexBuffer.Destroy( _device );
        mVertexBuffer.Destroy( _device );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SubMeshSkinned::Create( Device& _device )
    {
        if( mIndices.empty() || mVertices.empty() ){ return; }

        const VkMemoryPropertyFlags memPropertyFlags = ( mHostVisible ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        const VkDeviceSize requiredVertexSize = sizeof( mVertices[0] ) * mVertices.size();
        if( mVertexBuffer.mBuffer == VK_NULL_HANDLE || mVertexBuffer.mSize < requiredVertexSize )
        {
            mVertexBuffer.Destroy( _device );
            mVertexBuffer.Create( _device, requiredVertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memPropertyFlags );
            _device.AddDebugName( (uint64_t)mVertexBuffer.mBuffer, "skinnned mesh vertex buffer" );
            _device.AddDebugName( (uint64_t)mVertexBuffer.mMemory, "skinnned mesh vertex buffer" );
        }

        const VkDeviceSize requiredIndexSize = sizeof( mIndices[0] ) * mIndices.size();
        if( mIndexBuffer.mBuffer == VK_NULL_HANDLE || mIndexBuffer.mSize < requiredIndexSize )
        {
            mIndexBuffer.Destroy( _device );
            mIndexBuffer.Create( _device, requiredIndexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memPropertyFlags );
            _device.AddDebugName( (uint64_t)mIndexBuffer.mBuffer, "mesh index buffer" );
            _device.AddDebugName( (uint64_t)mIndexBuffer.mMemory, "mesh index buffer" );
        }

        if( mHostVisible )
        {
            mIndexBuffer.SetData( _device, mIndices.data(), requiredIndexSize );
            mVertexBuffer.SetData( _device, mVertices.data(), requiredVertexSize );
        }
        else
        {
            {
                Buffer stagingBuffer;
                stagingBuffer.Create( _device, requiredIndexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
                stagingBuffer.SetData( _device, mIndices.data(), requiredIndexSize );
                VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
                stagingBuffer.CopyBufferTo( cmd, mIndexBuffer.mBuffer, requiredIndexSize );
                _device.EndSingleTimeCommands( cmd );
                stagingBuffer.Destroy( _device );
            }
            {
                Buffer stagingBuffer2;
                stagingBuffer2.Create( _device, requiredVertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
                stagingBuffer2.SetData( _device, mVertices.data(), requiredVertexSize );
                VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
                stagingBuffer2.CopyBufferTo( cmd2, mVertexBuffer.mBuffer, requiredVertexSize );
                _device.EndSingleTimeCommands( cmd2 );
                stagingBuffer2.Destroy( _device );
            }
        }
    }
}