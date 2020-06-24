#include "game/singletons/fanGame.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Game::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.onGui = &Game::OnGui;
		_info.save = &Game::Save;
		_info.load = &Game::Load;
		_info.name = "game";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Init( EcsWorld& /*_world*/, EcsSingleton& _component ){
		Game& gameData = static_cast<Game&>( _component );
		gameData.state = STOPPED;
		gameData.spaceshipPrefab.Set( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Save( const EcsSingleton& _component, Json& _json )
	{
		const Game& gameData = static_cast<const Game&>( _component );
		Serializable::SavePrefabPtr( _json, "spaceship", gameData.spaceshipPrefab );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Load( EcsSingleton& _component, const Json& _json )
	{
		Game& gameData = static_cast<Game&>( _component );
		Serializable::LoadPrefabPtr( _json, "spaceship", gameData.spaceshipPrefab );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		Game& gameData = static_cast<Game&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			// names
			char buffer[32];
			gameData.name.copy( buffer, gameData.name.size() );
			buffer[gameData.name.size()] = '\0';
			if( ImGui::InputText( "name", buffer, IM_ARRAYSIZE( buffer ) ) )
			{
				gameData.name = buffer;
			}

			// game state
			std::string stateStr =
				gameData.state == Game::STOPPED ? "stopped" :
				gameData.state == Game::PLAYING ? "playing" :
				gameData.state == Game::PAUSED ? "paused" : "error";
			ImGui::Text( "game state : %s", stateStr.c_str() );

			ImGui::FanPrefab( "spaceship", gameData.spaceshipPrefab );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}