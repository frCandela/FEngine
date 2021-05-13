#pragma once

#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"

namespace fan
{
    struct Mesh;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RenderDataModel
    {
        Mesh* mMesh;
        glm::mat4 mModelMatrix;
        glm::mat4 mNormalMatrix;
        glm::vec4 mColor;
        uint32_t  mShininess;
        uint32_t  mTextureIndex;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RenderDataDirectionalLight
    {
        glm::vec4 mDirection;
        glm::vec4 mAmbiant;
        glm::vec4 mDiffuse;
        glm::vec4 mSpecular;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RenderDataPointLight
    {
        glm::vec4    mPosition;
        glm::vec4    mDiffuse;
        glm::vec4    mSpecular;
        glm::vec4    mAmbiant;
        glm::float32 mConstant;
        glm::float32 mLinear;
        glm::float32 mQuadratic;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UniformDirectionalLight
    {
        alignas( 16 ) glm::vec4 mDirection;
        alignas( 16 ) glm::vec4 mAmbiant;
        alignas( 16 ) glm::vec4 mDiffuse;
        alignas( 16 ) glm::vec4 mSpecular;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UniformPointLight
    {
        alignas( 16 ) glm::vec4   mPosition;
        alignas( 16 ) glm::vec4   mDiffuse;
        alignas( 16 ) glm::vec4   mSpecular;
        alignas( 16 ) glm::vec4   mAmbiant;
        alignas( 4 ) glm::float32 mConstant;
        alignas( 4 ) glm::float32 mLinear;
        alignas( 4 ) glm::float32 mQuadratic;
        alignas( 4 ) glm::float32 _0; // padding
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UniformLights
    {
        UniformDirectionalLight mDirLights[RenderGlobal::sMaximumNumDirectionalLight];
        UniformPointLight       mPointlights[RenderGlobal::sMaximumNumPointLights];
        uint32_t                mDirLightsNum;
        uint32_t                mPointLightNum;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DynamicUniformMatrices
    {
        glm::mat4 mModelMat;
        glm::mat4 mNormalMat;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UniformCameraPosition
    {
        glm::vec3 mCameraPosition = glm::vec3( 0, 0, 0 );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UniformViewProj
    {
        glm::mat4 mView;
        glm::mat4 mProj;
    };

    //==================================================================================================================================================================================================
    // material data for drawing a mesh
    //==================================================================================================================================================================================================
    struct DynamicUniformsMaterial
    {
        glm::vec4  mColor = glm::vec4( 1 );
        glm::int32 mShininess;
    };

    //==================================================================================================================================================================================================
    // All uniforms data for the models drawing
    //==================================================================================================================================================================================================
    struct UniformsModelDraw
    {
        void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

        AlignedMemory <DynamicUniformsMaterial> mDynamicUniformsMaterial;
        AlignedMemory <DynamicUniformMatrices>  mDynamicUniformsMatrices;
        UniformLights                           mUniformsLights;
        UniformViewProj                         mUniformsProjView;
        UniformCameraPosition                   mUniformsCameraPosition;
    };

    //==================================================================================================================================================================================================
    // references a mesh that must be drawn on the screen
    //==================================================================================================================================================================================================
    struct DrawData
    {
        Mesh* mMesh;
        uint32_t mTextureIndex;
    };

    struct RenderPass;
    struct FrameBuffer;
    struct DescriptorImages;

    //==================================================================================================================================================================================================
    // Models drawing data for the rendering engine
    //==================================================================================================================================================================================================
    struct DrawModels
    {
        Pipeline              mPipeline;
        Shader                mFragmentShader;
        Shader                mVertexShader;
        UniformsModelDraw     mUniforms;
        DescriptorUniforms    mDescriptorUniforms;
        DescriptorSampler     mDescriptorSampler;
        Sampler               mSamplerTextures;
        CommandBuffer         mCommandBuffers;
        std::vector<DrawData> mDrawData;

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
        void SetDrawData( Device& _device,
                          const uint32_t _imagesCount,
                          const std::vector<RenderDataModel>& _drawData );
        void SetPointLights( const std::vector<RenderDataPointLight>& _lightData );
        void SetDirectionalLights( const std::vector<RenderDataDirectionalLight>& _lightData );
        PipelineConfig GetPipelineConfig( DescriptorImages& _imagesDescriptor ) const;
    };
}