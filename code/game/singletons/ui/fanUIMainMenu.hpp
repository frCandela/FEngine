#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct UIMainMenu : public EcsSingleton
    {
        ECS_SINGLETON( UIMainMenu )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _this );
        static void Save( const EcsSingleton& _this, Json& _json );
        static void Load( EcsSingleton& _this, const Json& _json );

        static void EnableNodeAndChildren( SceneNode& _root, const bool _enable );

        // signal targets
        void ToogleMainMenu();

        // slots
        static void HideAll( EcsSingleton& _this );
        static void ShowMainMenu( EcsSingleton& _this );
        static void ShowOptions( EcsSingleton& _this );
        static void ShowCredits( EcsSingleton& _this );

        ComponentPtr<SceneNode> mMainMenuNode;
        ComponentPtr<SceneNode> mOptionsNode;
        ComponentPtr<SceneNode> mCreditsNode;

        bool mVisible;
    };
}