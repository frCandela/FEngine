#pragma once

#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"
#include "render/core/fanSampler.hpp"

namespace fan
{
    struct Mesh2D;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RenderDataMesh2D
    {
        Mesh2D* mMesh;
        glm::vec2 mPosition;
        glm::vec2 mScale;
        glm::vec2 mUvOffset;
        glm::vec2 mUvScale;
        glm::vec4 mColor;
        uint32_t  mTextureIndex;
        int       mDepth = 0;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DynamicUniformUIVert
    {
        glm::vec2 mPosition;
        glm::vec2 mScale;
        glm::vec4 mColor;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DynamicUniformUIFrag
    {
        glm::vec2 mUvOffset;
        glm::vec2 mUvScale;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UniformsUI
    {
        void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

        AlignedMemory <DynamicUniformUIVert> mDynamicUniformsVert;
        AlignedMemory <DynamicUniformUIFrag> mDynamicUniformsFrag;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UIDrawData
    {
        Mesh2D* mMesh;
        uint32_t mTextureIndex;
    };

    struct FrameBuffer;
    struct RenderPass;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DrawUI
    {
        Pipeline                mPipeline;
        Shader                  mFragmentShader;
        Shader                  mVertexShader;
        DescriptorUniforms      mDescriptor;
        UniformsUI              mUniforms;
        DescriptorSampler       mDescriptorSampler;
        Sampler                 mSampler;
        CommandBuffer           mCommandBuffers;
        std::vector<UIDrawData> mDrawData;

        void Create( Device& _device, uint32_t _imagesCount );
        void Destroy( Device& _device );
        void BindDescriptors( VkCommandBuffer _commandBuffer,
                              const size_t _indexFrame,
                              const uint32_t _indexOffset );
        void UpdateUniformBuffers( Device& _device, const size_t _index );
        void RecordCommandBuffer( const size_t _index,
                                  RenderPass& _renderPass,
                                  FrameBuffer& _framebuffer,
                                  VkExtent2D _extent,
                                  DescriptorImages& _descriptorTextures );
        void BindTexture( VkCommandBuffer _commandBuffer,
                          const uint32_t _textureIndex,
                          DescriptorSampler& _descriptorSampler,
                          DescriptorImages& _descriptorTextures,
                          VkPipelineLayout _pipelineLayout );
        void SetUIDrawData( const std::vector<RenderDataMesh2D>& _drawData );
        PipelineConfig GetPipelineConfig( DescriptorImages& _descriptorImages ) const;
    };
}