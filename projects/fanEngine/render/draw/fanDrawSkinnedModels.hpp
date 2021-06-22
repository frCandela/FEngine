#pragma once

#include "fanDisableWarnings.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "core/math/fanMatrix4.hpp"
#include "render/draw/fanUniforms.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"

namespace fan
{
    struct SubSkinnedMesh;
    struct Skeleton;
    struct Texture;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RenderDataSkinnedModel
    {
        SubSkinnedMesh* mMesh;
        Matrix4 mOffsetMatrix[RenderGlobal::sMaxBones];
        Skeleton* mSkeleton;
        Texture * mTexture;
        glm::mat4 mModelMatrix;
        glm::mat4 mNormalMatrix;
        glm::vec4 mColor;
        uint32_t  mShininess;
    };

    //==================================================================================================================================================================================================
    // All uniforms data for the models drawing
    //==================================================================================================================================================================================================
    struct UniformsSkinnedModelDraw
    {
        void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

        AlignedMemory<DynamicUniformsMaterial> mDynamicUniformsMaterial;
        AlignedMemory<DynamicUniformMatrices>  mDynamicUniformsMatrices;
        AlignedMemory<DynamicUniformBones>     mDynamicUniformsBones;
        UniformLights                          mUniformsLights;
        UniformViewProj                        mUniformsProjView;
        UniformCameraPosition                  mUniformsCameraPosition;
    };

    struct RenderPass;
    struct FrameBuffer;
    struct DescriptorImages;

    //==================================================================================================================================================================================================
    // Models drawing data for the rendering engine
    //==================================================================================================================================================================================================
    struct DrawSkinnedModels
    {
        Pipeline                            mPipeline;
        Shader                              mFragmentShader;
        Shader                              mVertexShader;
        UniformsSkinnedModelDraw            mUniforms;
        DescriptorUniforms                  mDescriptorUniforms;
        DescriptorSampler                   mDescriptorSampler;
        Sampler                             mSamplerTextures;
        CommandBuffer                       mCommandBuffers;
        ResourcePtr<Texture>                mInvalidTexture;
        std::vector<RenderDataSkinnedModel> mDrawData;

        void Create( Device& _device, uint32_t _imagesCount, ResourcePtr<Texture> _invalidTexture );
        void Destroy( Device& _device );
        void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
        void UpdateUniformBuffers( Device& _device, const size_t _index );
        void RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent, DescriptorImages& _descriptorImages );
        void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler& _descriptorSampler, DescriptorImages& _descriptorImages, VkPipelineLayout _pipelineLayout );
        void SetDrawData( Device& _device, const uint32_t _imagesCount, const std::vector<RenderDataSkinnedModel>& _drawData );
        void SetPointLights( const std::vector<UniformPointLight>& _lightData );
        void SetDirectionalLights( const std::vector<UniformDirectionalLight>& _lightData );
        PipelineConfig GetPipelineConfig( DescriptorImages& _imagesDescriptor ) const;
    };
}