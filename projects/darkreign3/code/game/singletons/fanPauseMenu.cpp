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
#include "engine/ui/fanUIScaler.hpp"
#include "engine/ui/fanUIText.hpp"
#include "engine/ui/fanUIButton.hpp"

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
        pauseMenu.mHandle     = 0;
        pauseMenu.mBackground = nullptr;
        pauseMenu.mButton     = nullptr;
        pauseMenu.mHeader     = nullptr;
        pauseMenu.mOnResume.Clear();
        pauseMenu.mOnOptions.Clear();
        pauseMenu.mOnQuit.Clear();
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
            pauseMenu.mHandle = 0;
        }

        // root
        SceneNode& rootNode = scene.CreateSceneNode( "ui_pause_menu", &scene.GetRootNode() );
        pauseMenu.mHandle = rootNode.mHandle;
        EcsEntity rootEntity = _world.GetEntity( rootNode.mHandle );
        _world.AddComponent<UIAlign>( rootEntity );
        UITransform& rootTransform = _world.AddComponent<UITransform>( rootEntity );
        rootTransform.mSize = { 256, 256 };

        // background
        if( pauseMenu.mBackground != nullptr )
        {
            pauseMenu.mBackground->Instantiate( rootNode );
        }

        // header
        if( pauseMenu.mHeader != nullptr )
        {
            pauseMenu.mHeader->Instantiate( rootNode );
        }

        // close button
        if( pauseMenu.mCloseButton != nullptr )
        {
            SceneNode* nodeClose = pauseMenu.mCloseButton->Instantiate( rootNode );
            fanAssert( nodeClose != nullptr );
            EcsEntity entityClose = _world.GetEntity( nodeClose->mHandle );
            UIButton& buttonClose = _world.GetComponent<UIButton>( entityClose );
            buttonClose.mPressed.Connect( &PauseMenu::OnResume, &pauseMenu );
        }

        // buttons root
        SceneNode& buttonsRootNode = scene.CreateSceneNode( "buttons_layout", &rootNode );
        EcsEntity buttonsRootEntity = _world.GetEntity( buttonsRootNode.mHandle );
        _world.AddComponent<UIAlign>( buttonsRootEntity );
        _world.AddComponent<UITransform>( buttonsRootEntity );
        _world.AddComponent<UIScaler>( buttonsRootEntity );
        UILayout& buttonsLayout = _world.AddComponent<UILayout>( buttonsRootEntity );
        buttonsLayout.mGap = { 16, 16 };

        // buttons
        if( pauseMenu.mButton != nullptr )
        {
            // resume
            SceneNode* nodeResume = pauseMenu.mButton->Instantiate( buttonsRootNode );
            nodeResume->mName = "button_resume";
            UIText* textResume = SceneNode::FindComponentInChildren<UIText>( *nodeResume );
            fanAssert( textResume != nullptr );
            textResume->mText = "Resume";
            EcsEntity entityResume = _world.GetEntity( nodeResume->mHandle );
            UIButton& buttonResume = _world.GetComponent<UIButton>( entityResume );
            buttonResume.mPressed.Connect( &PauseMenu::OnResume, &pauseMenu );

            // options
            SceneNode* nodeOptions = pauseMenu.mButton->Instantiate( buttonsRootNode );
            nodeOptions->mName = "button_options";
            UIText* textOptions = SceneNode::FindComponentInChildren<UIText>( *nodeOptions );
            fanAssert( textOptions != nullptr );
            textOptions->mText = "Options";
            EcsEntity entityOptions = _world.GetEntity( nodeOptions->mHandle );
            UIButton& buttonOptions = _world.GetComponent<UIButton>( entityOptions );
            buttonOptions.mPressed.Connect( &PauseMenu::OnOptions, &pauseMenu );

            // quit
            SceneNode* nodeQuit = pauseMenu.mButton->Instantiate( buttonsRootNode );
            nodeQuit->mName = "button_quit";
            UIText* textQuit = SceneNode::FindComponentInChildren<UIText>( *nodeQuit );
            fanAssert( textQuit != nullptr );
            textQuit->mText = "Quit";
            EcsEntity entityQuit = _world.GetEntity( nodeQuit->mHandle );
            UIButton& buttonQuit = _world.GetComponent<UIButton>( entityQuit );
            buttonQuit.mPressed.Connect( &PauseMenu::OnQuit, &pauseMenu );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::OnResume()
    {
        Debug::Log("PauseMenu::OnResume");
        mOnResume.Emmit();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::OnOptions()
    {
        mOnOptions.Emmit();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::OnQuit()
    {
        mOnQuit.Emmit();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Show( EcsWorld& _world )
    {
        PauseMenu& pauseMenu = _world.GetSingleton<PauseMenu>();
        if( pauseMenu.mHandle != 0 )
        {
            SceneNode& rootNode = _world.GetComponent<SceneNode>( _world.GetEntity( pauseMenu.mHandle ) );
            SceneNode::ExecuteInChildren( rootNode, []( SceneNode& _node )
                                          {
                                              EcsWorld& world = *_node.mScene->mWorld;
                                              EcsEntity entity = world.GetEntity( _node.mHandle );
                                              world.AddTag<TagVisible>( entity );
                                              world.AddTag<TagEnabled>( entity );
                                          }
            );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Hide( EcsWorld& _world )
    {
        PauseMenu& pauseMenu = _world.GetSingleton<PauseMenu>();
        if( pauseMenu.mHandle != 0 )
        {
            SceneNode& rootNode = _world.GetComponent<SceneNode>( _world.GetEntity( pauseMenu.mHandle ) );
            SceneNode::ExecuteInChildren( rootNode, []( SceneNode& _node )
                                          {
                                              EcsWorld& world = *_node.mScene->mWorld;
                                              EcsEntity entity = world.GetEntity( _node.mHandle );
                                              world.RemoveTag<TagVisible>( entity );
                                              world.RemoveTag<TagEnabled>( entity );
                                          }
            );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Destroy( EcsWorld& _world )
    {
        PauseMenu& pauseMenu = _world.GetSingleton<PauseMenu>();
        if( pauseMenu.mHandle != 0 )
        {
            _world.Kill( _world.GetEntity( pauseMenu.mHandle ) );
            pauseMenu.mHandle = 0;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const PauseMenu& pauseMenu = static_cast<const PauseMenu&>( _singleton );
        Serializable::SaveResourcePtr( _json, "background", pauseMenu.mBackground );
        Serializable::SaveResourcePtr( _json, "button", pauseMenu.mButton );
        Serializable::SaveResourcePtr( _json, "header", pauseMenu.mHeader );
        Serializable::SaveResourcePtr( _json, "close_button", pauseMenu.mCloseButton );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PauseMenu::Load( EcsSingleton& _singleton, const Json& _json )
    {
        PauseMenu& pauseMenu = static_cast<PauseMenu&>( _singleton );
        Serializable::LoadResourcePtr( _json, "background", pauseMenu.mBackground );
        Serializable::LoadResourcePtr( _json, "button", pauseMenu.mButton );
        Serializable::LoadResourcePtr( _json, "header", pauseMenu.mHeader );
        Serializable::LoadResourcePtr( _json, "close_button", pauseMenu.mCloseButton );
    }
}