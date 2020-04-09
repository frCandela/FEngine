#include "game/singletonComponents/fanGame.hpp"

#include "ecs/fanEcsWorld.hpp"

#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( Game );

	//================================================================================================================================
	//================================================================================================================================
	void Game::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.init = &Game::Init;
		_info.onGui = &Game::OnGui;
		_info.save = &Game::Save;
		_info.load = &Game::Load;
		_info.name = "game";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Init( EcsWorld& _world, SingletonComponent& _component ){
		Game& gameData = static_cast<Game&>( _component );
		gameData.spaceshipPrefab = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::OnGui( SingletonComponent& _component )
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

			// spaceship prefab
			ImGui::FanPrefab( "spaceship", gameData.spaceshipPrefab );

		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Save( const SingletonComponent& _component, Json& _json )
	{
		const Game& gameData = static_cast<const Game&>( _component );
		Serializable::SavePrefabPtr( _json, "spaceship", gameData.spaceshipPrefab );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Load( SingletonComponent& _component, const Json& _json )
	{
		Game& gameData = static_cast<Game&>( _component );
		Serializable::LoadPrefabPtr( _json, "spaceship", gameData.spaceshipPrefab );
	}

	//================================================================================================================================
	// generates the spaceship entity from the game prefab
	//================================================================================================================================
	EntityID Game::SpawnSpaceship( EcsWorld& _world )
	{
		// spawn the spaceship	
		Game& game = _world.GetSingletonComponent< Game >();
		if( game.spaceshipPrefab != nullptr )
		{
			Scene& scene = _world.GetSingletonComponent<Scene>();
			SceneNode& spaceshipNode = *game.spaceshipPrefab->Instanciate( *scene.root );
			EntityID spaceshipID = _world.GetEntityID( spaceshipNode.handle );

			if( _world.HasComponent<Transform>( spaceshipID )
				&& _world.HasComponent<Rigidbody>( spaceshipID )
				&& _world.HasComponent<MotionState>( spaceshipID )
				&& _world.HasComponent<BoxShape>( spaceshipID ) )
			{
				// set initial position
				Transform& transform = _world.GetComponent<Transform>( spaceshipID );
				transform.SetPosition( btVector3( 0, 0, 4.f ) );

				// add rigidbody to the physics world
				PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
				Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( spaceshipID );
				MotionState& motionState = _world.GetComponent<MotionState>( spaceshipID );
				BoxShape& boxShape = _world.GetComponent<BoxShape>( spaceshipID );
				rigidbody.SetCollisionShape( &boxShape.boxShape );
				rigidbody.SetMotionState( &motionState.motionState );
				physicsWorld.AddRigidbody( rigidbody, spaceshipNode.handle );

				// registers physics callbacks
				CollisionManager& collisionManager = _world.GetSingletonComponent<CollisionManager>();
				rigidbody.onContactStarted.Connect( &CollisionManager::OnSpaceShipContact, &collisionManager );

				return spaceshipID;
			}
			else
			{
				Debug::Error()
					<< "Game: spaceship prefab is missing a component" << "\n"
					<< "component needed: Transform, Rigidbody, MotionState, BoxShape" << Debug::Endl();
				return 0;
			}
		}
		else
		{
			Debug::Error() << game.name << " spaceship prefab is null" << Debug::Endl();
			return 0;
		}
	}
}