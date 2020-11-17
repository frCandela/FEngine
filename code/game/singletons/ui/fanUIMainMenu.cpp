#include "core/fanDebug.hpp"
#include "game/singletons/ui/fanUIMainMenu.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/ui/fanUIRenderer.hpp"
#include "engine/components/ui/fanUIButton.hpp"
#include "engine/fanSceneTags.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::SetInfo( EcsSingletonInfo& _info )
    {
        _info.load        = &UIMainMenu::Load;
        _info.save        = &UIMainMenu::Save;
        _info.mSlots.push_back( new Slot<>("hide_all", &UIMainMenu::HideAll ) );
        _info.mSlots.push_back( new Slot<>("show_main", &UIMainMenu::ShowMainMenu ) );
        _info.mSlots.push_back( new Slot<>("show_option", &UIMainMenu::ShowOptions ) );
        _info.mSlots.push_back( new Slot<>("show_credits", &UIMainMenu::ShowCredits ) );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::Init( EcsWorld& _world, EcsSingleton& _singleton )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _singleton );
        menu.mMainMenuNode.Init( _world );
        menu.mOptionsNode.Init( _world );
        menu.mCreditsNode.Init( _world );
        menu.mVisible = true;


        Signal<>& toogleMainMenuSignal = *Input::Get().Manager().CreateKeyboardEvent( "toogle_main_menu",
                                                                                      Keyboard::ESCAPE );
        toogleMainMenuSignal.Clear();
        toogleMainMenuSignal.Connect( &UIMainMenu::ToogleMainMenu, &menu );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ToogleMainMenu()
    {
        bool visible = ! mVisible;
        HideAll( *this );
        if( visible )
        {
            ShowMainMenu( *this );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::EnableNodeAndChildren( SceneNode& _root, const bool _enable )
    {
        EcsWorld& world = * _root.mScene->mWorld;
        std::vector<SceneNode*> descendants;
        SceneNode::GetDescendantsOf( _root, descendants );
        for( SceneNode * node : descendants )
        {
            fanAssert( node != nullptr );
            EcsEntity entity = world.GetEntity( node->mHandle );
            if( _enable )
            {
                world.AddTag<TagUIEnabled>( entity );
                world.AddTag<TagUIVisible>( entity );
            }
            else
            {
                world.RemoveTag<TagUIEnabled>( entity );
                world.RemoveTag<TagUIVisible>( entity );
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::HideAll( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        if( menu.mMainMenuNode.IsValid() ){ EnableNodeAndChildren( *menu.mMainMenuNode, false ); }
        if( menu.mOptionsNode.IsValid() ) { EnableNodeAndChildren( *menu.mOptionsNode, false );  }
        if( menu.mCreditsNode.IsValid() ) { EnableNodeAndChildren( *menu.mCreditsNode, false );  }
        menu.mVisible = false;
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowMainMenu( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        HideAll( menu );
        if( menu.mMainMenuNode.IsValid() ){ EnableNodeAndChildren( *menu.mMainMenuNode, true ); }
        menu.mVisible = true;
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowOptions( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        HideAll( menu );
        if( menu.mOptionsNode.IsValid() ){ EnableNodeAndChildren( *menu.mOptionsNode, true ); }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowCredits( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        HideAll( menu );
        if( menu.mCreditsNode.IsValid() ){ EnableNodeAndChildren( *menu.mCreditsNode, true ); }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const UIMainMenu& menu = static_cast<const UIMainMenu&>( _singleton );
        Serializable::SaveComponentPtr( _json, "main_menu_node", menu.mMainMenuNode );
        Serializable::SaveComponentPtr( _json, "options_node", menu.mOptionsNode );
        Serializable::SaveComponentPtr( _json, "credits_node", menu.mCreditsNode );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::Load( EcsSingleton& _singleton, const Json& _json )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _singleton );
        Serializable::LoadComponentPtr( _json, "main_menu_node", menu.mMainMenuNode );
        Serializable::LoadComponentPtr( _json, "options_node", menu.mOptionsNode );
        Serializable::LoadComponentPtr( _json, "credits_node", menu.mCreditsNode );
    }
}
