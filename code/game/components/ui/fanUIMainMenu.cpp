#include "core/fanDebug.hpp"
#include "game/components/ui/fanUIMainMenu.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void UIMainMenu::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::MainMenu16;
		_info.group = EngineGroups::Game;
		_info.onGui = &UIMainMenu::OnGui;
		_info.load = &UIMainMenu::Load;
		_info.save = &UIMainMenu::Save;
		_info.editorPath = "game/ui/";
		_info.name = "main menu";
        _info.mSlots.push_back( new Slot<>("show_main", &UIMainMenu::OnShowMain ) );
        _info.mSlots.push_back( new Slot<>("show_option", &UIMainMenu::OnShowOptions ) );
        _info.mSlots.push_back( new Slot<>("show_credits", &UIMainMenu::OnShowCredits ) );
        _info.mSlots.push_back( new Slot<>("play", &UIMainMenu::OnPlay ) );
        _info.mSlots.push_back( new Slot<>("quit", &UIMainMenu::OnQuit ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMainMenu::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UIMainMenu& mainMenu = static_cast<UIMainMenu&>( _component );
        mainMenu.mPlayNode.Init( _world );
        mainMenu.mOptionsNode.Init( _world );
        mainMenu.mCreditsNode.Init( _world );
        mainMenu.mQuitNode.Init( _world );
	}

    //================================================================================================================================
    //================================================================================================================================
    void UIMainMenu::OnShowMain( EcsComponent&  )   { Debug::Log( "OnShowMain");}
    void UIMainMenu::OnShowOptions( EcsComponent&  ){ Debug::Log( "OnShowOptions");}
    void UIMainMenu::OnShowCredits( EcsComponent&  ){ Debug::Log( "OnShowCredits");}
    void UIMainMenu::OnPlay( EcsComponent&  )       { Debug::Log( "OnPlay");}
    void UIMainMenu::OnQuit( EcsComponent&  )       { Debug::Log( "OnQuit");}

	//================================================================================================================================
	//================================================================================================================================
	void UIMainMenu::Save( const EcsComponent& _component, Json& _json )
	{
        const UIMainMenu& mainMenu = static_cast<const UIMainMenu&>( _component );
        Serializable::SaveComponentPtr( _json, "play_node", mainMenu.mPlayNode );
        Serializable::SaveComponentPtr( _json, "options_node", mainMenu.mOptionsNode );
        Serializable::SaveComponentPtr( _json, "credits_node", mainMenu.mCreditsNode );
        Serializable::SaveComponentPtr( _json, "quit_node", mainMenu.mQuitNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMainMenu::Load( EcsComponent& _component, const Json& _json )
	{
        UIMainMenu& mainMenu = static_cast<UIMainMenu&>( _component );
        Serializable::LoadComponentPtr( _json, "play_node", mainMenu.mPlayNode );
        Serializable::LoadComponentPtr( _json, "options_node", mainMenu.mOptionsNode );
        Serializable::LoadComponentPtr( _json, "credits_node", mainMenu.mCreditsNode );
        Serializable::LoadComponentPtr( _json, "quit_node", mainMenu.mQuitNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIMainMenu::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
        UIMainMenu& mainMenu = static_cast<UIMainMenu&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
		    ImGui::FanComponent("play", mainMenu.mPlayNode);
            ImGui::FanComponent( "options", mainMenu.mOptionsNode );
            ImGui::FanComponent( "credits", mainMenu.mCreditsNode );
            ImGui::FanComponent( "quit", mainMenu.mQuitNode );
		} ImGui::PopItemWidth();
	}
}
