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
        static void Show( EcsWorld& _world );
        static void Hide( EcsWorld& _world );
        static void Destroy( EcsWorld& _world );
        void OnResume();
        void OnOptions();
        void OnQuit();

        EcsHandle mHandle = 0;

        ResourcePtr <Prefab> mBackground;
        ResourcePtr <Prefab> mButton;
        ResourcePtr <Prefab> mHeader;
        ResourcePtr <Prefab> mCloseButton;

        Signal<> mOnResume;
        Signal<> mOnOptions;
        Signal<> mOnQuit;
    };
}