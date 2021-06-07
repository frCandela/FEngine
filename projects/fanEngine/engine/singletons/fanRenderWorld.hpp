#pragma once

#include "core/shapes/fanAABB.hpp"
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/fanColor.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "render/fanRenderer.hpp"
#include "engine/fanFullscreen.hpp"

namespace fan
{
    struct Mesh;

    //==================================================================================================================================================================================================
    // contains all the render data of the world for the renderer
    //==================================================================================================================================================================================================
    class RenderWorld : public EcsSingleton
    {
    ECS_SINGLETON( RenderWorld )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void PostInit( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _singleton, Json& _json );
        static void Load( EcsSingleton& _singleton, const Json& _json );

        std::vector<RenderDataModel>         mModels;
        std::vector<RenderDataSkinnedModel>  mSkinnedModels;
        std::vector<RenderDataMesh2D>        mUIModels;
        std::vector<UniformPointLight>       mPointLights;
        std::vector<UniformDirectionalLight> mDirectionalLights;

        glm::vec2 mTargetSize; // render target size in pixels
        bool      mIsHeadless;

        ResourcePtr<Mesh> mParticlesMesh;
        Color             mFilterColor;
        Color             mClearColor;

        FullScreen mFullscreen;
    };
}