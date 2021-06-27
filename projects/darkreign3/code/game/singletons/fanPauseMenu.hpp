#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanPrefab.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct PauseMenu : public EcsSingleton
    {
    ECS_SINGLETON( PauseMenu )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        static void Instantiate( EcsWorld& _world );

        EcsHandle mHandle = 0;

        ResourcePtr<Prefab> mHeader;
        ResourcePtr<Prefab> mBackground;
    };
}