#include "render/draw/fanDrawUI.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanTexture.hpp"

namespace fan
{
    //================================================================================================================================
    //================================================================================================================================
    void DrawUI::Create( Device& _device, uint32_t _imagesCount )
    {
        mUniforms.Create( _device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
        mSampler.Create( _device, 0, 1, VK_FILTER_NEAREST );
        mDescriptorSampler.Create( _device, mSampler.mSampler );

        mDescriptorTransform.AddDynamicUniformBinding( _device,
                                                       _imagesCount,
                                                       VK_SHADER_STAGE_VERTEX_BIT,
                                                       mUniforms.mDynamicUniforms.Size(),
                                                       mUniforms.mDynamicUniforms.Alignment() );
        mDescriptorTransform.Create( _device, _imagesCount );
    }

    //================================================================================================================================
    //================================================================================================================================
    void DrawUI::Destroy( Device& _device )
    {
        mDescriptorTransform.Destroy( _device );
        mDescriptorSampler.Destroy( _device );
        mSampler.Destroy( _device );
    }

    //================================================================================================================================
    //================================================================================================================================
    void
    DrawUI::BindDescriptors( VkCommandBuffer _commandBuffer,
                             const size_t _indexFrame,
                             const uint32_t _indexOffset )
    {
        std::vector<VkDescriptorSet> descriptors    = {
                mDescriptorTransform.mDescriptorSets[_indexFrame]
        };
        std::vector<uint32_t>        dynamicOffsets = {
                _indexOffset * static_cast<uint32_t>( mUniforms.mDynamicUniforms.Alignment() )
        };

        vkCmdBindDescriptorSets(
                _commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mPipeline.mPipelineLayout,
                0,
                static_cast<uint32_t>( descriptors.size() ),
                descriptors.data(),
                static_cast<uint32_t>( dynamicOffsets.size() ),
                dynamicOffsets.data()
        );
    }

    //================================================================================================================================
    //================================================================================================================================
    PipelineConfig DrawUI::GetPipelineConfig( DescriptorImages& _descriptorImages ) const
    {
        PipelineConfig config( mVertexShader, mFragmentShader );
        config.bindingDescription    = UIVertex::GetBindingDescription();
        config.attributeDescriptions = UIVertex::GetAttributeDescriptions();
        config.descriptorSetLayouts  = {
                mDescriptorTransform.mDescriptorSetLayout,
                _descriptorImages.mDescriptorSetLayout,
                mDescriptorSampler.mDescriptorSetLayout
        };

        return config;
    }

    //================================================================================================================================
    //================================================================================================================================
    void DrawUI::UpdateUniformBuffers( Device& _device, const size_t _index )
    {
        mDescriptorTransform.SetData( _device,
                                      0,
                                      _index,
                                      &mUniforms.mDynamicUniforms[0],
                                      mUniforms.mDynamicUniforms.Alignment() *
                                      mUniforms.mDynamicUniforms.Size(),
                                      0 );
    }

    //================================================================================================================================
    //================================================================================================================================
    void
    DrawUI::RecordCommandBuffer( const size_t _index,
                                 RenderPass& _renderPass,
                                 FrameBuffer& _framebuffer,
                                 VkExtent2D _extent,
                                 DescriptorImages& _descriptorTextures )
    {
        VkCommandBuffer                commandBuffer                = mCommandBuffers.mBuffers[_index];
        VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo(
                _renderPass.mRenderPass,
                _framebuffer.mFrameBuffers[_index] );
        VkCommandBufferBeginInfo       commandBufferBeginInfo       = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

        if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
        {
            mPipeline.Bind( commandBuffer, _extent );

            VkDeviceSize offsets[] = { 0 };

            for( uint32_t meshIndex = 0; meshIndex < mDrawData.size(); meshIndex++ )
            {
                UIDrawData drawData = mDrawData[meshIndex];
                Mesh2D* mesh = drawData.mesh;
                VkBuffer vertexBuffers[] = { mesh->mVertexBuffer[mesh->mCurrentBuffer].mBuffer };
                BindDescriptors( commandBuffer, _index, meshIndex );
                vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
                BindTexture( commandBuffer,
                             drawData.textureIndex,
                             mDescriptorSampler,
                             _descriptorTextures,
                             mPipeline.mPipelineLayout );
                vkCmdDraw( commandBuffer, static_cast<uint32_t>( mesh->mVertices.size() ), 1, 0, 0 );
            }

            if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
            {
                Debug::Get()
                        << Debug::Severity::error
                        << "Could not record command buffer "
                        << _index
                        << "."
                        << Debug::Endl();
            }
        }
        else
        {
            Debug::Get()
                    << Debug::Severity::error
                    << "Could not record command buffer "
                    << _index
                    << "."
                    << Debug::Endl();
        }
    }

    //================================================================================================================================
    //================================================================================================================================
    void DrawUI::BindTexture( VkCommandBuffer _commandBuffer,
                              const uint32_t _textureIndex,
                              DescriptorSampler& _descriptorSampler,
                              DescriptorImages& _descriptorTextures,
                              VkPipelineLayout _pipelineLayout )
    {
        assert( _textureIndex < _descriptorTextures.mDescriptorSets.size() );

        std::vector<VkDescriptorSet> descriptors = {
                _descriptorTextures.mDescriptorSets[_textureIndex], _descriptorSampler.mDescriptorSet
        };

        vkCmdBindDescriptorSets(
                _commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                _pipelineLayout,
                1,
                static_cast<uint32_t>( descriptors.size() ),
                descriptors.data(),
                0,
                nullptr
        );
    }

    //================================================================================================================================
    //================================================================================================================================
    void DrawUI::SetUIDrawData( const std::vector<RenderDataUIMesh>& _drawData )
    {
        mDrawData.resize( _drawData.size() );
        for( int meshIndex = 0; meshIndex < (int)_drawData.size(); meshIndex++ )
        {
            const RenderDataUIMesh& uiData = _drawData[meshIndex];

            mDrawData[meshIndex].mesh                      = uiData.mesh;
            mDrawData[meshIndex].textureIndex              = uiData.textureIndex;
            mUniforms.mDynamicUniforms[meshIndex].position = uiData.position;
            mUniforms.mDynamicUniforms[meshIndex].scale    = uiData.scale;
            mUniforms.mDynamicUniforms[meshIndex].color    = uiData.color;
        }
    }

    //================================================================================================================================
    // UiUniforms
    //================================================================================================================================
    void UniformsUI::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
    {
        // Calculate required alignment based on minimum device offset alignment
        size_t minUboAlignment      = (size_t)_minUniformBufferOffsetAlignment;
        size_t dynamicAlignmentVert = sizeof( DynamicUniformUIVert );
        if( minUboAlignment > 0 )
        {
            dynamicAlignmentVert = ( ( sizeof( DynamicUniformUIVert ) + minUboAlignment - 1 ) &
                                     ~( minUboAlignment - 1 ) );
        }
        mDynamicUniforms.SetAlignement( dynamicAlignmentVert );
        mDynamicUniforms.Resize( 256 );
    }
}