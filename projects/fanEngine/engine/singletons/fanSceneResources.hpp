#pragma once

#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
    class PrefabManager;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SceneResources : public EcsSingleton
    {
    ECS_SINGLETON( SceneResources )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        static void SetupResources( PrefabManager& _prefabManager );
        void SetPointers( PrefabManager* _prefabManager );

        PrefabManager* mPrefabManager = nullptr;
    };
}