#pragma once

#include "core/shapes/fanAABB.hpp"
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/fanColor.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"
#include "engine/fanFullscreen.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // contains all the render data of the world for the renderer
    //==================================================================================================================================================================================================
    class RenderWorld : public EcsSingleton
    {
    ECS_SINGLETON( RenderWorld )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _singleton, Json& _json );
        static void Load( EcsSingleton& _singleton, const Json& _json );

        std::vector<RenderDataModel>            drawData;
        std::vector<RenderDataMesh2D>           uiDrawData;
        std::vector<RenderDataPointLight>       pointLights;
        std::vector<RenderDataDirectionalLight> directionalLights;

        Mesh* mParticlesMesh = nullptr;
        glm::vec2 mTargetSize; // render target size in pixels
        bool      mIsHeadless;

        Color mFilterColor;
        Color mClearColor;

        FullScreen mFullscreen;
    };
}