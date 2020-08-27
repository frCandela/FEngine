#include "game/singletons/fanGame.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Game::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Joystick16;
		_info.mGroup = EngineGroups::Game;
		_info.onGui  = &Game::OnGui;
		_info.save   = &Game::Save;
		_info.load   = &Game::Load;
		_info.mName  = "game";
	}

	//========================================================================================================
	//========================================================================================================
	void Game::Init( EcsWorld& /*_world*/, EcsSingleton& _component ){
		Game& gameData = static_cast<Game&>( _component );
		gameData.mSpaceshipPrefab.Set( nullptr );
	}

	//========================================================================================================
	//========================================================================================================
	void Game::Save( const EcsSingleton& _component, Json& _json )
	{
		const Game& gameData = static_cast<const Game&>( _component );
		Serializable::SavePrefabPtr( _json, "spaceship", gameData.mSpaceshipPrefab );
	}

	//========================================================================================================
	//========================================================================================================
	void Game::Load( EcsSingleton& _component, const Json& _json )
	{
		Game& gameData = static_cast<Game&>( _component );
		Serializable::LoadPrefabPtr( _json, "spaceship", gameData.mSpaceshipPrefab );
	}

	//========================================================================================================
	//========================================================================================================
	void Game::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		Game& gameData = static_cast<Game&>( _component );
		ImGui::FanPrefab( "spaceship", gameData.mSpaceshipPrefab );
	}
}