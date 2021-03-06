#include "render/draw/fanDrawModels.hpp"

#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanFrameBuffer.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::Create( Device& _device, uint32_t _imagesCount, ResourcePtr<Texture> _invalidTexture )
    {
        mInvalidTexture = _invalidTexture;

        mSamplerTextures.Create( _device, 0, 8, VK_FILTER_LINEAR );
        mDescriptorSampler.Create( _device, mSamplerTextures.mSampler );

        const size_t initialDrawDataSize = 256;
        mDrawData.reserve( initialDrawDataSize );
        mUniforms.Create( _device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
        mUniforms.mDynamicUniformsMatrices.Resize( initialDrawDataSize );
        mUniforms.mDynamicUniformsMaterial.Resize( initialDrawDataSize );

        mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( UniformViewProj ) );
        mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, mUniforms.mDynamicUniformsMatrices.Size(), mUniforms.mDynamicUniformsMatrices.Alignment() );
        mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformCameraPosition ) );
        mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, mUniforms.mDynamicUniformsMaterial.Size(), mUniforms.mDynamicUniformsMaterial.Alignment() );
        mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformLights ) );
        mDescriptorUniforms.Create( _device, _imagesCount );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::Destroy( Device& _device )
    {
        mPipeline.Destroy( _device );
        mDescriptorUniforms.Destroy( _device );
        mDescriptorSampler.Destroy( _device );
        mSamplerTextures.Destroy( _device );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    PipelineConfig DrawModels::GetPipelineConfig( DescriptorImages& _imagesDescriptor ) const
    {
        PipelineConfig config( mVertexShader, mFragmentShader );

        config.bindingDescription    = Vertex::GetBindingDescription();
        config.attributeDescriptions = Vertex::GetAttributeDescriptions();
        config.descriptorSetLayouts  = { mDescriptorUniforms.mDescriptorSetLayout, _imagesDescriptor.mDescriptorSetLayout, mDescriptorSampler.mDescriptorSetLayout };

        return config;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
    {
        std::vector<VkDescriptorSet> descriptors    = {
                mDescriptorUniforms.mDescriptorSets[_indexFrame]
        };
        std::vector<uint32_t>        dynamicOffsets = {
                _indexOffset * static_cast<uint32_t>( mUniforms.mDynamicUniformsMatrices.Alignment() ),
                _indexOffset * static_cast<uint32_t>( mUniforms.mDynamicUniformsMaterial.Alignment() )
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::UpdateUniformBuffers( Device& _device, const size_t _index )
    {
        mDescriptorUniforms.SetData( _device, 0, _index, &mUniforms.mUniformsProjView, sizeof( UniformViewProj ), 0 );
        mDescriptorUniforms.SetData( _device, 1, _index, &mUniforms.mDynamicUniformsMatrices[0], mUniforms.mDynamicUniformsMatrices.Alignment() * mUniforms.mDynamicUniformsMatrices.Size(), 0 );
        mDescriptorUniforms.SetData( _device, 2, _index, &mUniforms.mUniformsCameraPosition, sizeof( UniformCameraPosition ), 0 );
        mDescriptorUniforms.SetData( _device, 3, _index, &mUniforms.mDynamicUniformsMaterial[0], mUniforms.mDynamicUniformsMaterial.Alignment() * mUniforms.mDynamicUniformsMaterial.Size(), 0 );
        mDescriptorUniforms.SetData( _device, 4, _index, &mUniforms.mUniformsLights, sizeof( UniformLights ), 0 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UniformsModelDraw::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
    {
        // Calculate required alignment based on minimum device offset alignment
        size_t minUboAlignment      = (size_t)_minUniformBufferOffsetAlignment;
        size_t dynamicAlignmentVert = sizeof( DynamicUniformMatrices );
        size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
        if( minUboAlignment > 0 )
        {
            dynamicAlignmentVert = ( ( sizeof( DynamicUniformMatrices ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
            dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
        }

        mDynamicUniformsMatrices.SetAlignement( dynamicAlignmentVert );
        mDynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

        mDynamicUniformsMatrices.Resize( 256 );
        mDynamicUniformsMaterial.Resize( 256 );

        for( int uniformIndex = 0; uniformIndex < (int)mDynamicUniformsMaterial.Size(); uniformIndex++ )
        {
            mDynamicUniformsMaterial[uniformIndex].mColor     = glm::vec4( 1 );
            mDynamicUniformsMaterial[uniformIndex].mShininess = 1;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent, DescriptorImages& _descriptorImages )
    {
        VkCommandBuffer                commandBuffer                = mCommandBuffers.mBuffers[_index];
        VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( _renderPass.mRenderPass, _framebuffer.mFrameBuffers[_index] );
        VkCommandBufferBeginInfo       commandBufferBeginInfo       = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

        fanAssert( mInvalidTexture != nullptr );

        if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
        {
            mPipeline.Bind( commandBuffer, _extent );
            for( uint32_t meshIndex = 0; meshIndex < mDrawData.size(); meshIndex++ )
            {
                RenderDataModel& drawData = mDrawData[meshIndex];
                SubMesh & subMesh  = *drawData.mMesh;

                Buffer& buffer = subMesh.mVertexBuffer[subMesh.mCurrentBuffer];
                if( buffer.mBuffer == VK_NULL_HANDLE ){ continue; }

                const uint32_t textureIndex = drawData.mTexture == nullptr ? mInvalidTexture->mIndex : drawData.mTexture->mIndex;
                BindTexture( commandBuffer, textureIndex, mDescriptorSampler, _descriptorImages, mPipeline.mPipelineLayout );
                BindDescriptors( commandBuffer, _index, meshIndex );
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindVertexBuffers( commandBuffer, 0, 1, &subMesh.mVertexBuffer[subMesh.mCurrentBuffer].mBuffer, offsets );
                vkCmdBindIndexBuffer( commandBuffer, subMesh.mIndexBuffer[subMesh.mCurrentBuffer].mBuffer, 0, VK_INDEX_TYPE_UINT32 );
                vkCmdDrawIndexed( commandBuffer, static_cast<uint32_t>( subMesh.mIndices.size() ), 1, 0, 0, 0 );
            }

            if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
            {
                Debug::Error() << "Could not record command buffer " << _index << "." << Debug::Endl();
            }
        }
        else
        {
            Debug::Error() << "Could not record command buffer " << _index << "." << Debug::Endl();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::SetDrawData( Device& _device, const uint32_t _imagesCount, const std::vector<RenderDataModel>& _drawData )
    {
        if( _drawData.size() > mDrawData.capacity() )
        {
            Debug::Warning( "Resizing draw data arrays" );
            vkDeviceWaitIdle( _device.mDevice );
            const size_t newSize = 2 * _drawData.size();
            mDrawData.reserve( newSize );
            mUniforms.mDynamicUniformsMatrices.Resize( newSize );
            mUniforms.mDynamicUniformsMaterial.Resize( newSize );
            mDescriptorUniforms.ResizeDynamicUniformBinding( _device, _imagesCount, mUniforms.mDynamicUniformsMatrices.Size(), mUniforms.mDynamicUniformsMatrices.Alignment(), 1 );
            mDescriptorUniforms.ResizeDynamicUniformBinding( _device, _imagesCount, mUniforms.mDynamicUniformsMaterial.Size(), mUniforms.mDynamicUniformsMaterial.Alignment(), 3 );
            mDescriptorUniforms.UpdateDescriptorSets( _device, _imagesCount );
        }

        mDrawData.clear();
        for( int dataIndex = 0; dataIndex < (int)_drawData.size(); dataIndex++ )
        {
            const RenderDataModel& data = _drawData[dataIndex];

            fanAssert( data.mMesh != nullptr );
            fanAssert( !data.mMesh->mIndices.empty() );

            // Transform
            mUniforms.mDynamicUniformsMatrices[dataIndex].mModelMat  = data.mModelMatrix;
            mUniforms.mDynamicUniformsMatrices[dataIndex].mNormalMat = data.mNormalMatrix;

            // material
            mUniforms.mDynamicUniformsMaterial[dataIndex].mColor     = data.mColor;
            mUniforms.mDynamicUniformsMaterial[dataIndex].mShininess = data.mShininess;

            // Mesh
            mDrawData.push_back( { data.mMesh, data.mTexture } );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::SetPointLights( const std::vector<UniformPointLight>& _lightData )
    {
        fanAssert( _lightData.size() < RenderGlobal::sMaximumNumPointLights );
        mUniforms.mUniformsLights.mPointLightNum = (uint32_t)_lightData.size();
        std::memcpy( mUniforms.mUniformsLights.mPointlights, _lightData.data(), _lightData.size() * sizeof( UniformPointLight ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::SetDirectionalLights( const std::vector<UniformDirectionalLight>& _lightData )
    {
        fanAssert( _lightData.size() < RenderGlobal::sMaximumNumDirectionalLight );
        mUniforms.mUniformsLights.mDirLightsNum = (uint32_t)_lightData.size();
        std::memcpy( mUniforms.mUniformsLights.mDirLights, _lightData.data(), _lightData.size() * sizeof( UniformDirectionalLight ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawModels::BindTexture( VkCommandBuffer _commandBuffer,
                                  const uint32_t _textureIndex,
                                  DescriptorSampler& _descriptorSampler,
                                  DescriptorImages& _descriptorImages,
                                  VkPipelineLayout _pipelineLayout )
    {
        fanAssert( _textureIndex < _descriptorImages.mDescriptorSets.size() );

        std::vector<VkDescriptorSet> descriptors = { _descriptorImages.mDescriptorSets[_textureIndex], _descriptorSampler.mDescriptorSet };

        vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 1,
                                 static_cast<uint32_t>( descriptors.size() ), descriptors.data(), 0, nullptr );
    }
}