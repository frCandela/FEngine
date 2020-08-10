#include "core/fanDebug.hpp"
#include "game/components/ui/fanMainMenu.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void MainMenu::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NONE;
		_info.group = EngineGroups::Game;
		_info.onGui = &MainMenu::OnGui;
		_info.load = &MainMenu::Load;
		_info.save = &MainMenu::Save;
		_info.editorPath = "game/ui/";
		_info.name = "main menu";
        _info.mSlots.push_back( new Slot<>("show_main", &MainMenu::OnShowMain ) );
        _info.mSlots.push_back( new Slot<>("show_option", &MainMenu::OnShowOptions ) );
        _info.mSlots.push_back( new Slot<>("show_credits", &MainMenu::OnShowCredits ) );
        _info.mSlots.push_back( new Slot<>("play", &MainMenu::OnPlay ) );
        _info.mSlots.push_back( new Slot<>("quit", &MainMenu::OnQuit ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenu::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        MainMenu& mainMenu = static_cast<MainMenu&>( _component );
        mainMenu.mPlayNode.Init( _world );
        mainMenu.mOptionsNode.Init( _world );
        mainMenu.mCreditsNode.Init( _world );
        mainMenu.mQuitNode.Init( _world );
	}

    //================================================================================================================================
    //================================================================================================================================
    void MainMenu::OnShowMain( EcsComponent&  )   { Debug::Log("OnShowMain");}
    void MainMenu::OnShowOptions( EcsComponent&  ){ Debug::Log("OnShowOptions");}
    void MainMenu::OnShowCredits( EcsComponent&  ){ Debug::Log("OnShowCredits");}
    void MainMenu::OnPlay( EcsComponent&  )       { Debug::Log("OnPlay");}
    void MainMenu::OnQuit( EcsComponent&  )       { Debug::Log("OnQuit");}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenu::Save( const EcsComponent& _component, Json& _json )
	{
        const MainMenu& mainMenu = static_cast<const MainMenu&>( _component );
        Serializable::SaveComponentPtr( _json, "play_node", mainMenu.mPlayNode );
        Serializable::SaveComponentPtr( _json, "options_node", mainMenu.mOptionsNode );
        Serializable::SaveComponentPtr( _json, "credits_node", mainMenu.mCreditsNode );
        Serializable::SaveComponentPtr( _json, "quit_node", mainMenu.mQuitNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenu::Load( EcsComponent& _component, const Json& _json )
	{
        MainMenu& mainMenu = static_cast<MainMenu&>( _component );
        Serializable::LoadComponentPtr( _json, "play_node", mainMenu.mPlayNode );
        Serializable::LoadComponentPtr( _json, "options_node", mainMenu.mOptionsNode );
        Serializable::LoadComponentPtr( _json, "credits_node", mainMenu.mCreditsNode );
        Serializable::LoadComponentPtr( _json, "quit_node", mainMenu.mQuitNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenu::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
        MainMenu& mainMenu = static_cast<MainMenu&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
		    ImGui::FanComponent("play", mainMenu.mPlayNode);
            ImGui::FanComponent( "options", mainMenu.mOptionsNode );
            ImGui::FanComponent( "credits", mainMenu.mCreditsNode );
            ImGui::FanComponent( "quit", mainMenu.mQuitNode );
		} ImGui::PopItemWidth();
	}
}
