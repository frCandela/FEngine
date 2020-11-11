#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Game::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Joystick16;
		_info.mGroup = EngineGroups::Game;
		_info.onGui  = &Game::OnGui;
		_info.mName  = "game";
	}

	//========================================================================================================
	//========================================================================================================
	void Game::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		Game& gameData = static_cast<Game&>( _component );
		ImGui::FanPrefab( "spaceship", gameData.mSpaceshipPrefab );
	}
}