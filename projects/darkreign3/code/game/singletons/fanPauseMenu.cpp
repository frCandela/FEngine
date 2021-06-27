#include "game/singletons/fanPauseMenu.hpp"
#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIAlign.hpp"
#include "engine/ui/fanUILayout.hpp"
#include "engine/ui/fanUIRenderer.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &PauseMenu::Save;
        _info.load = &PauseMenu::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        PauseMenu& pauseMenu = static_cast<PauseMenu&>( _singleton );
        pauseMenu.mHandle = 0;
        pauseMenu.mHeader = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Instantiate( EcsWorld& _world )
    {
        PauseMenu& pauseMenu = _world.GetSingleton<PauseMenu>();
        Scene    & scene     = _world.GetSingleton<Scene>();
        if( pauseMenu.mHandle != 0 )
        {
            _world.Kill( _world.GetEntity( pauseMenu.mHandle ) );
        }

        // root
        SceneNode& rootNode = scene.CreateSceneNode( "ui_pause_menu", &scene.GetRootNode() );
        pauseMenu.mHandle = rootNode.mHandle;
        EcsEntity rootEntity = _world.GetEntity( rootNode.mHandle );
        _world.AddComponent<UIAlign>( rootEntity );
        _world.AddComponent<UITransform>( rootEntity );

        // background
        if( pauseMenu.mBackground != nullptr )
        {
            pauseMenu.mBackground->Instantiate( rootNode );
        }

        // buttons root
        SceneNode& buttonsRootNode = scene.CreateSceneNode( "buttons_root", &rootNode );
        EcsEntity buttonsRootEntity = _world.GetEntity( buttonsRootNode.mHandle );
        _world.AddComponent<UIAlign>( buttonsRootEntity );
        _world.AddComponent<UITransform>( buttonsRootEntity );
        _world.AddComponent<UILayout>( buttonsRootEntity );

        // Header text
        if( pauseMenu.mHeader != nullptr )
        {
            pauseMenu.mHeader->Instantiate( buttonsRootNode );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const PauseMenu& pauseMenu = static_cast<const PauseMenu&>( _singleton );
        Serializable::SaveResourcePtr( _json, "header", pauseMenu.mHeader );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Load( EcsSingleton& _singleton, const Json& _json )
    {
        PauseMenu& pauseMenu = static_cast<PauseMenu&>( _singleton );
        Serializable::LoadResourcePtr( _json, "header", pauseMenu.mHeader );
    }
}