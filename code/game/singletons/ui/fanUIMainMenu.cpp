#include "core/fanDebug.hpp"
#include "game/singletons/ui/fanUIMainMenu.hpp"

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
        _info.mSlots.push_back( new Slot<>("show_main", &UIMainMenu::OnShowMain ) );
        _info.mSlots.push_back( new Slot<>("show_option", &UIMainMenu::OnShowOptions ) );
        _info.mSlots.push_back( new Slot<>("show_credits", &UIMainMenu::OnShowCredits ) );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::Init( EcsWorld& _world, EcsSingleton& _singleton )
    {
        UIMainMenu& mainMenu = static_cast<UIMainMenu&>( _singleton );
        mainMenu.mMainMenuNode.Init( _world );
        mainMenu.mOptionsNode.Init( _world );
        mainMenu.mCreditsNode.Init( _world );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::OnShowMain( EcsSingleton&  )   { Debug::Log( "OnShowMain");}
    void UIMainMenu::OnShowOptions( EcsSingleton&  ){ Debug::Log( "OnShowOptions");}
    void UIMainMenu::OnShowCredits( EcsSingleton&  ){ Debug::Log( "OnShowCredits");}

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const UIMainMenu& mainMenu = static_cast<const UIMainMenu&>( _singleton );
        Serializable::SaveComponentPtr( _json, "main_menu_node", mainMenu.mMainMenuNode );
        Serializable::SaveComponentPtr( _json, "options_node", mainMenu.mOptionsNode );
        Serializable::SaveComponentPtr( _json, "credits_node", mainMenu.mCreditsNode );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::Load( EcsSingleton& _singleton, const Json& _json )
    {
        UIMainMenu& mainMenu = static_cast<UIMainMenu&>( _singleton );
        Serializable::LoadComponentPtr( _json, "main_menu_node", mainMenu.mMainMenuNode );
        Serializable::LoadComponentPtr( _json, "options_node", mainMenu.mOptionsNode );
        Serializable::LoadComponentPtr( _json, "credits_node", mainMenu.mCreditsNode );
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        UIMainMenu& mainMenu = static_cast<UIMainMenu&>( _singleton );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::FanComponent("main menu", mainMenu.mMainMenuNode);
            ImGui::FanComponent( "options", mainMenu.mOptionsNode );
            ImGui::FanComponent( "credits", mainMenu.mCreditsNode );
        } ImGui::PopItemWidth();
    }
}
