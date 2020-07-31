#include "render/fanMeshManager2.hpp"

#include <filesystem>
#include "render/fanMesh2.hpp"
#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    MeshManager2::~MeshManager2()
    {
        if( ! mMeshes.empty() || ! mDestroyList.empty() )
        {
            Debug::Warning() << "Mesh manager was not cleared before destroy" << Debug::Endl();
        }
    }

    //========================================================================================================
    //========================================================================================================
    Mesh2* MeshManager2::GetMesh( const std::string& _path ) const
    {
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
        for( const MeshData& meshData : mMeshes )
        {
            if( meshData.mMesh->mPath == cleanPath )
            {
                return meshData.mMesh;
            }
        }
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    Mesh2* MeshManager2::LoadMesh( const std::string& _path )
    {
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();

        // Load
        Mesh2* mesh = new Mesh2();
        if( mesh->LoadFromFile( cleanPath ) )
        {
            mesh->mIndex = (int)mMeshes.size();
            mMeshes.push_back( { mesh } );
            return mesh;
        }
        delete mesh;
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager2::AddMesh( Mesh2 * mesh, const std::string& _name )
    {
        assert( mesh != nullptr );
        mesh->mIndex = (int)mMeshes.size();
        mesh->mPath = _name;
        mMeshes.push_back( { mesh } );
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager2::Remove( const std::string& _path )
    {
        const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
        for( int          meshIndex = 0; meshIndex < mMeshes.size(); meshIndex++ )
        {
            if( mMeshes[meshIndex].mMesh->mPath == cleanPath )
            {
                mMeshes.rbegin()->mMesh->mIndex = meshIndex;
                delete mMeshes[meshIndex].mMesh;

                for( int i = 0; i < SwapChain::s_maxFramesInFlight; i++ )
                {
                    mDestroyList.push_back( mMeshes[meshIndex].mIndexBuffer[i] );
                    mDestroyList.push_back( mMeshes[meshIndex].mVertexBuffer[i] );
                }

                mMeshes[meshIndex] = mMeshes[mMeshes.size() - 1];
                mMeshes.pop_back();
                return;
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager2::Clear( Device& _device )
    {
        for ( MeshData& meshData : mMeshes )
        {
            delete meshData.mMesh;

            for( int i = 0 ; i < SwapChain::s_maxFramesInFlight; i++)
            {
                meshData.mIndexBuffer[i].Destroy( _device );
                meshData.mVertexBuffer[i].Destroy( _device );
            }
        }
        mMeshes.clear();

        DestroyBuffers( _device );
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager2::DestroyBuffers( Device& _device )
    {
        for( Buffer& buffer : mDestroyList )
        {
            buffer.Destroy( _device );
        }
        mDestroyList.clear();
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager2::ResolvePtr( ResourcePtr< Mesh2 >& _resourcePtr )
    {
        assert( ! _resourcePtr.IsValid() );

        Mesh2Ptr& meshPtr = static_cast< Mesh2Ptr& >( _resourcePtr );
        Mesh2 * mesh = GetMesh( meshPtr.GetPath() );
        if ( mesh != nullptr )
        {
            meshPtr = mesh;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void MeshManager2::GenerateGpuBuffers( Device& _device, MeshData& _meshData )
    {
        assert( _meshData.mMesh != nullptr );
        Mesh2& mesh = *_meshData.mMesh;

        if ( mesh.mIndices.empty() ) { return; }

        _meshData.mCurrentBuffer = ( _meshData.mCurrentBuffer + 1 ) % 3;

        const VkMemoryPropertyFlags memPropertyFlags = ( mesh.mHostVisible ?
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        Buffer& vertexBuffer = _meshData.mVertexBuffer[ _meshData.mCurrentBuffer ];
        const VkDeviceSize requiredVertexSize = sizeof( mesh.mVertices[ 0 ] ) * mesh.mVertices.size();
        if ( vertexBuffer.mBuffer == VK_NULL_HANDLE || vertexBuffer.mSize < requiredVertexSize )
        {
            vertexBuffer.Destroy( _device );
            _meshData.mVertexBuffer[ _meshData.mCurrentBuffer ].Create(
                    _device,
                    requiredVertexSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    memPropertyFlags
            );
            _device.AddDebugName( (uint64_t)vertexBuffer.mBuffer, "mesh vertex buffer" );
            _device.AddDebugName( (uint64_t)vertexBuffer.mMemory, "mesh vertex buffer" );
        }

        Buffer& indexBuffer = _meshData.mIndexBuffer[ _meshData.mCurrentBuffer ];
        const VkDeviceSize requiredIndexSize = sizeof( mesh.mIndices[ 0 ] ) * mesh.mIndices.size();
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

        if ( mesh.mHostVisible )
        {
            indexBuffer.SetData( _device, mesh.mIndices.data(), requiredIndexSize );
            vertexBuffer.SetData( _device, mesh.mVertices.data(), requiredVertexSize );
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
                stagingBuffer.SetData( _device, mesh.mIndices.data(), requiredIndexSize );
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
                stagingBuffer2.SetData( _device, mesh.mVertices.data(), requiredVertexSize );
                VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
                stagingBuffer2.CopyBufferTo( cmd2, vertexBuffer.mBuffer, requiredVertexSize );
                _device.EndSingleTimeCommands( cmd2 );
                stagingBuffer2.Destroy( _device );
            }
        }
    }
}