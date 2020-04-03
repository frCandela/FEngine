#include "game/singletonComponents/fanGameReference.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "game/fanGame.hpp"

#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( GameReference );

	//================================================================================================================================
	//================================================================================================================================
	void GameReference::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.init = &GameReference::Init;
		_info.onGui = &GameReference::OnGui;
		_info.save = &GameReference::Save;
		_info.load = &GameReference::Load;
		_info.name = "game";
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameReference::Init( EcsWorld& _world, SingletonComponent& _component ){}

	//================================================================================================================================
	//================================================================================================================================
	void GameReference::OnGui( SingletonComponent& _component )
	{
		Game& game = *static_cast<const GameReference&>( _component ).game;

		ImGui::Indent(); ImGui::Indent();
		{
			// names
			char buffer[32];
			game.name.copy( buffer, game.name.size() );
			buffer[game.name.size()] = '\0';
			if( ImGui::InputText( "name", buffer, IM_ARRAYSIZE( buffer ) ) )
			{
				game.name = buffer;
			}

			// spaceship prefab
			ImGui::FanPrefab( "spaceship", game.spaceship );

			// game state
			std::string state =
				game.state == Game::STOPPED ? "stopped" :
				game.state == Game::PLAYING ? "playing" :
				game.state == Game::PAUSED ? "paused" : "error";
			ImGui::Text( "game state : %s", state.c_str() );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameReference::Save( const SingletonComponent& _component, Json& _json )
	{
		const Game& game = * static_cast<const GameReference&>( _component ).game;
		Serializable::SavePrefabPtr( _json, "spaceship", game.spaceship );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameReference::Load( SingletonComponent& _component, const Json& _json )
	{
		Game& game = *static_cast< GameReference&>( _component ).game;
		Serializable::LoadPrefabPtr( _json, "spaceship", game.spaceship );
	}
}