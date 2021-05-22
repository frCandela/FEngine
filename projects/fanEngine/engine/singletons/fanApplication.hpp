#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "engine/resources/fanSceneResourcePtr.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // interface between the ecs and the rest of the application
    //==================================================================================================================================================================================================
    struct Application : public EcsSingleton
    {
    ECS_SINGLETON( Application )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _this );

        // slots
        static void OnQuit( EcsSingleton& _this );

        // signals connected to the application
        Signal<> mOnQuit;
    };
}