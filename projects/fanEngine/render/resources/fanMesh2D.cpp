#include "fanMesh2D.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Mesh2D::LoadFromVertices( const std::vector<UIVertex>& _vertices )
    {
        mVertices        = _vertices;
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mesh2D::Create( Device& _device )
    {
        if( mVertices.empty() ){ return; }

        const VkMemoryPropertyFlags memPropertyFlags = ( mHostVisible ?
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT :
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        const VkDeviceSize requiredVertexSize = sizeof( mVertices[0] ) * mVertices.size();
        if( mVertexBuffer.mBuffer == VK_NULL_HANDLE || mVertexBuffer.mSize < requiredVertexSize )
        {
            mVertexBuffer.Destroy( _device );
            mVertexBuffer.Create(
                    _device,
                    requiredVertexSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    memPropertyFlags
            );
            _device.AddDebugName( (uint64_t)mVertexBuffer.mBuffer, "mesh2D vertex buffer" );
            _device.AddDebugName( (uint64_t)mVertexBuffer.mMemory, "mesh2D vertex buffer" );
        }

        if( mHostVisible )
        {
            mVertexBuffer.SetData( _device, mVertices.data(), requiredVertexSize );
        }
        else
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
            stagingBuffer2.CopyBufferTo( cmd2, mVertexBuffer.mBuffer, requiredVertexSize );
            _device.EndSingleTimeCommands( cmd2 );
            stagingBuffer2.Destroy( _device );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mesh2D::Destroy( Device& _device )
    {
        mVertexBuffer.Destroy( _device );
    }
}