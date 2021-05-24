#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "engine/resources/fanSceneResourcePtr.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{
    class Resources;
    class Window;
    struct Cursor;

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

        void Setup( Resources* _resource );

        // signals connected to the application
        Signal<> mOnQuit;
        Resources* mResources;
        Cursor * mCurrentCursor = nullptr;
    };
}