#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "render/fanMeshManager.hpp"


namespace fan
{
    struct Device;

    //========================================================================================================
    //========================================================================================================
    struct RenderResources : public EcsSingleton
    {
        ECS_SINGLETON( RenderResources )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );

        MeshManager  * mMeshManager = nullptr;
    };
}

