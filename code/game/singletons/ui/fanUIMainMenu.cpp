#include "core/fanDebug.hpp"
#include "game/singletons/ui/fanUIMainMenu.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon       = ImGui::IconType::MainMenu16;
        _info.mGroup      = EngineGroups::Game;
        _info.onGui       = &UIMainMenu::OnGui;
        _info.load        = &UIMainMenu::Load;
        _info.save        = &UIMainMenu::Save;
        _info.mName       = "main menu";
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
        toogleMainMenuSignal.Connect( &UIMainMenu::ToogleMainMenu, &menu );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ToogleMainMenu()
    {
        mVisible = ! mVisible;
        HideAll( *this );
        if( mVisible )
        {
            ShowMainMenu( *this );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowNodeAndChildren( SceneNode& _root, const bool _visible )
    {
        EcsWorld& world = * _root.mScene->mWorld;
        std::vector<SceneNode*> descendants;
        SceneNode::GetDescendantsOf( _root, descendants );
        descendants.push_back( &_root );
        for( SceneNode * node : descendants )
        {
            fanAssert( node != nullptr );
            EcsEntity entity = world.GetEntity( node->mHandle );
            if( world.HasComponent<UIRenderer>( entity ) )
            {
                UIRenderer& renderer = world.GetComponent<UIRenderer>( entity );
                renderer.mVisible = _visible;
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::HideAll( EcsSingleton& _this )
    {
        const UIMainMenu& menu = static_cast<const UIMainMenu&>( _this );
        if( menu.mMainMenuNode.IsValid() ){ ShowNodeAndChildren( *menu.mMainMenuNode, false ); }
        if( menu.mOptionsNode.IsValid() ) { ShowNodeAndChildren( *menu.mOptionsNode, false );  }
        if( menu.mCreditsNode.IsValid() ) { ShowNodeAndChildren( *menu.mCreditsNode, false );  }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowMainMenu( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        HideAll( menu );
        if( menu.mMainMenuNode.IsValid() ){ ShowNodeAndChildren( *menu.mMainMenuNode, true ); }
        menu.mVisible = true;
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowOptions( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        HideAll( menu );
        if( menu.mOptionsNode.IsValid() ){ ShowNodeAndChildren( *menu.mOptionsNode, true ); }
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::ShowCredits( EcsSingleton& _this )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _this );
        HideAll( menu );
        if( menu.mCreditsNode.IsValid() ){ ShowNodeAndChildren( *menu.mCreditsNode, true ); }
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

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _singleton );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::FanComponent( "main menu", menu.mMainMenuNode);
            ImGui::FanComponent( "options", menu.mOptionsNode );
            ImGui::FanComponent( "credits", menu.mCreditsNode );

            ImGui::Spacing();

            if( ImGui::Button("hide all")){ HideAll( menu ); }
            ImGui::SameLine();
            if( ImGui::Button("show main menu")){ ShowMainMenu( menu ); }
            ImGui::SameLine();
            if( ImGui::Button("show options")){ ShowOptions( menu ); }
            ImGui::SameLine();
            if( ImGui::Button("show credits")){ ShowCredits( menu ); }

        } ImGui::PopItemWidth();
    }
}
