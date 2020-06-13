#include "game/singletons/fanGame.hpp"

#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "game/singletons/fanCollisionManager.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanPlayerController.hpp"
#include "network/components/fanClientGameData.hpp"

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
		gameData.frameIndex = 0;
		gameData.logicDelta = 1.f / 60.f;
		gameData.frameStart = 0;
		gameData.timeScaleDelta = 0.f;
		gameData.timeScaleIncrement = gameData.logicDelta / 20.f; // it takes 20 frames to time scale one frame ( 5% faster/slower )
	}

	//================================================================================================================================
	// Client must adjust it frame index to be in sync with the server
	// if the delta is small enough, use timescale
	// if it's too big, offsets the frame index directly
	//================================================================================================================================
	void Game::OnShiftFrameIndex( const int _framesDelta )
	{
		if( std::abs( _framesDelta ) > ClientGameData::maxFrameDeltaBeforeShift )
		{
			frameIndex += _framesDelta;
		}
		else
		{
			timeScaleDelta = _framesDelta * logicDelta;
		}
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
	// Generates the spaceship entity from the game prefab
	// PlayerInput component causes the ship to be driven by inputs ( forward, left, right, boost etc. )
	// PlayerController automatically updates the PlayerInput with local inputs from mouse & keyboard
	//================================================================================================================================
	EcsHandle Game::SpawnSpaceship( EcsWorld& _world, const bool _hasPlayerInput, const bool _hasPlayerController )
	{
		// spawn the spaceship	
		Game& game = _world.GetSingleton< Game >();
		if( game.spaceshipPrefab != nullptr )
		{
			Scene& scene = _world.GetSingleton<Scene>();
			SceneNode& spaceshipNode = *game.spaceshipPrefab->Instanciate( scene.GetRootNode() );
			EcsEntity spaceshipID = _world.GetEntity( spaceshipNode.handle );
			
			if( _hasPlayerInput )
			{
				_world.AddComponent<PlayerInput>( spaceshipID );
			}
			if( _hasPlayerController )
			{
				_world.AddComponent<PlayerController>( spaceshipID );
			}

			if( _world.HasComponent<Transform>( spaceshipID )
				&& _world.HasComponent<Rigidbody>( spaceshipID )
				&& _world.HasComponent<MotionState>( spaceshipID )
				&& _world.HasComponent<BoxShape>( spaceshipID ) )
			{
				// set initial position
				Transform& transform = _world.GetComponent<Transform>( spaceshipID );
				transform.SetPosition( btVector3( 0, 0, 4.f ) );

				// add rigidbody to the physics world
				PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
				Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( spaceshipID );
				MotionState& motionState = _world.GetComponent<MotionState>( spaceshipID );
				BoxShape& boxShape = _world.GetComponent<BoxShape>( spaceshipID );
				rigidbody.SetCollisionShape( boxShape.boxShape );
				rigidbody.SetMotionState( motionState.motionState );
				rigidbody.rigidbody->setWorldTransform( transform.transform );
				physicsWorld.dynamicsWorld->addRigidBody( rigidbody.rigidbody );

				// registers physics callbacks
				CollisionManager& collisionManager = _world.GetSingleton<CollisionManager>();
				rigidbody.onContactStarted.Connect( &CollisionManager::OnSpaceShipContact, &collisionManager );

				return spaceshipNode.handle;
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
			ImGui::Text( "frame index: %d", gameData.frameIndex );

			// game state
			std::string stateStr =
				gameData.state == Game::STOPPED ? "stopped" :
				gameData.state == Game::PLAYING ? "playing" :
				gameData.state == Game::PAUSED ? "paused" : "error";
			ImGui::Text( "game state : %s", stateStr.c_str() );

			ImGui::FanPrefab( "spaceship", gameData.spaceshipPrefab );
			ImGui::DragFloat( "timescale", &gameData.timeScaleDelta, 0.1f );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}