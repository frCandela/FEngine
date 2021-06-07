#pragma once

#include "fanGlm.hpp"
#include "render/fanRenderGlobal.hpp"

//======================================================================================================================================================================================================
//======================================================================================================================================================================================================
namespace fan
{
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
}