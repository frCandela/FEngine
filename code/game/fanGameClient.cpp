#include "game/fanGameClient.hpp"

#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"

#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/systems/fanUpdateTimers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanRaycastUI.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/ui/fanUILayout.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/singletons/fanSceneResources.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/singletons/fanRenderDebug.hpp"

#include "network/fanPacket.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientRollback.hpp"
#include "network/systems/fanClientUpdates.hpp"
#include "network/systems/fanClientSendReceive.hpp"
#include "network/systems/fanTimeout.hpp"
#include "network/systems/fanRollback.hpp"

#include "game/fanGameTags.hpp"
#include "game/singletons/fanSunLight.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanCollisionManager.hpp"
#include "game/singletons/fanSolarEruption.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"
#include "game/systems/fanUpdateGameCamera.hpp"
#include "game/systems/fanUpdateWeapons.hpp"
#include "game/systems/fanUpdatePlayerInput.hpp"
#include "game/systems/fanUpdateEnergy.hpp"
#include "game/systems/fanUpdateGameUI.hpp"
#include "game/systems/fanParticlesOcclusion.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanDamage.hpp"
#include "game/components/ui/fanUIMainMenu.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameClient::GameClient( const std::string _name )
	{
		// base components
		world.AddComponentType<SceneNode>();
		world.AddComponentType<Transform>();
		world.AddComponentType<DirectionalLight>();
		world.AddComponentType<PointLight>();
		world.AddComponentType<MeshRenderer>();
		world.AddComponentType<Material>();
		world.AddComponentType<Camera>();
		world.AddComponentType<ParticleEmitter>();
		world.AddComponentType<Particle>();
		world.AddComponentType<Rigidbody>();
		world.AddComponentType<MotionState>();
		world.AddComponentType<BoxShape>();
		world.AddComponentType<SphereShape>();
		world.AddComponentType<UITransform>();
		world.AddComponentType<UIRenderer>();
		world.AddComponentType<Bounds>();
		world.AddComponentType<ExpirationTime>();
		world.AddComponentType<FollowTransform>();
		world.AddComponentType<UIProgressBar>();
		world.AddComponentType<UIFollowTransform>();
		world.AddComponentType<UIButton>();
        world.AddComponentType<UILayout>();
		// game components
		world.AddComponentType<Planet>();
		world.AddComponentType<SpaceShip>();
		world.AddComponentType<PlayerInput>();
		world.AddComponentType<Weapon>();
		world.AddComponentType<Bullet>();
		world.AddComponentType<Battery>();
		world.AddComponentType<SolarPanel>();
		world.AddComponentType<Health>();
		world.AddComponentType<SpaceshipUI>();
		world.AddComponentType<Damage>();
		world.AddComponentType<PlayerController>();
        world.AddComponentType<UIMainMenu>();
		// network components
		world.AddComponentType<ReliabilityLayer>();
		world.AddComponentType<ClientConnection>();
		world.AddComponentType<ClientReplication>();
		world.AddComponentType<ClientRPC>();
		world.AddComponentType<ClientGameData>();
		world.AddComponentType<ClientRollback>();
		world.AddComponentType<LinkingContextUnregisterer>();

		// base singleton components
		world.AddSingletonType<Scene>();
        world.AddSingletonType<RenderResources>();
        world.AddSingletonType<SceneResources>();
		world.AddSingletonType<RenderWorld>();
		world.AddSingletonType<PhysicsWorld>();
		world.AddSingletonType<ScenePointers>();
		world.AddSingletonType<RenderDebug>();
		world.AddSingletonType<Mouse>();
		// game singleton components
		world.AddSingletonType<SunLight>();
		world.AddSingletonType<GameCamera>();
		world.AddSingletonType<CollisionManager>();
		world.AddSingletonType<Game>();
		world.AddSingletonType<SolarEruption>();
		world.AddSingletonType<SpawnManager>();
		world.AddSingletonType<ClientNetworkManager>();
		// network singleton components		
		world.AddSingletonType<LinkingContext>();
		world.AddSingletonType<Time>();
		
		world.AddTagType<tag_sunlight_occlusion>();
		
		// @hack
		Game& gameSingleton = world.GetSingleton<Game>();
		gameSingleton.gameClient = this;
		gameSingleton.name = _name;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Start()
	{
		ClientNetworkManager& netManager = world.GetSingleton<ClientNetworkManager>();
		netManager.Start( world );

		// Init game
		world.Run<S_RegisterAllRigidbodies>();
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );

        Game& game = world.GetSingleton<Game>();
        MeshManager& meshManager = *world.GetSingleton<RenderResources>().mMeshManager;
        SunLight& sunLight = world.GetSingleton<SunLight>();
        RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
        meshManager.Add( sunLight.mesh, "sunlight_mesh_" + game.name );
        meshManager.Add( renderWorld.particlesMesh, "particles_mesh_" + game.name );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Stop()
	{
		// clears the physics world
		world.Run<S_UnregisterAllRigidbodies>();

		// clears the particles mesh
		RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
        SunLight& sunLight = world.GetSingleton<SunLight>();
        MeshManager& meshManager = *world.GetSingleton<RenderResources>().mMeshManager;
        meshManager.Remove( sunLight.mesh->mPath );
        meshManager.Remove( renderWorld.particlesMesh->mPath );

		GameCamera::DeleteGameCamera( world );

		ClientNetworkManager& netManager = world.GetSingleton<ClientNetworkManager>();
		netManager.Stop( world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::RollbackResimulate()
	{
		ClientNetworkManager& netManager = world.GetSingleton<ClientNetworkManager>();
		const EcsEntity persistentID = world.GetEntity( netManager.persistentHandle );
		ClientGameData& gameData = world.GetComponent<ClientGameData>( persistentID );
		if( !gameData.spaceshipSynced )
		{
			Time& time = world.GetSingleton<Time>();
			const FrameIndex lastFrame = time.frameIndex;
			const FrameIndex firstFrame = gameData.lastServerState.frameIndex;
			world.Run<S_RollbackInit>();
			Debug::Highlight() << "rollback to frame " << firstFrame << Debug::Endl();

			// Rollback at the frame we took the snapshot of the player game state
			time.frameIndex = firstFrame;

			// reset world to first frame		
			PhysicsWorld& physicsWorld = world.GetSingleton<PhysicsWorld>();
			physicsWorld.Reset();
			world.Run<S_RollbackRestoreState>( firstFrame );
			const EcsEntity spaceshipID = world.GetEntity( gameData.spaceshipHandle );
			Rigidbody& rigidbody = world.GetComponent<Rigidbody>( spaceshipID );
			physicsWorld.dynamicsWorld->removeRigidBody( rigidbody.rigidbody );
			physicsWorld.dynamicsWorld->addRigidBody( rigidbody.rigidbody );
			rigidbody.ClearForces();
			Transform& transform = world.GetComponent<Transform>( spaceshipID );
			rigidbody.SetVelocity( gameData.lastServerState.velocity );
			rigidbody.SetAngularVelocity( gameData.lastServerState.angularVelocity );
			transform.SetPosition( gameData.lastServerState.position );
			transform.SetRotationEuler( gameData.lastServerState.orientation );

			// Clear previous states & saves the last correct server state
			gameData.previousLocalStates = std::queue< PacketPlayerGameState >();
			gameData.previousLocalStates.push( gameData.lastServerState );

			// resimulate the last frames of input of the player
			const float delta = time.logicDelta;
			while( time.frameIndex < lastFrame )
			{
				time.frameIndex++;

				world.Run<S_RollbackRestoreState>( time.frameIndex );

				world.Run<S_MovePlanets>( delta );
				world.Run<S_MoveSpaceships>( delta );

				world.Run<S_SynchronizeMotionStateFromTransform>();
				physicsWorld.dynamicsWorld->stepSimulation( time.logicDelta, 10, Time::s_physicsDelta );
				world.Run<S_SynchronizeTransformFromMotionState>();

				world.Run<S_ClientSaveState>( delta );
				world.Run<S_RollbackStateSave>( delta );
			}

			gameData.spaceshipSynced = true;
			assert( time.frameIndex == lastFrame );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Step( const float _delta )
	{		
		Time& time = world.GetSingleton<Time>();

		{
			SCOPED_PROFILE( scene_update );			
			world.Run<S_ClientReceive>( _delta );
			world.Run<S_RollbackRemoveOldStates>();

			RollbackResimulate();

			if( _delta > 0.f )
			{
				time.frameIndex++;
			}

			world.Run<S_ProcessTimedOutPackets>();
			world.Run<S_ClientDetectServerTimeout>( _delta );
			world.Run<S_ClientRunReplication>(		_delta );
			SpawnManager::Update( world );

			// update
			world.Run<S_RefreshPlayerInput>( _delta );
			world.Run<S_ClientSaveInput>( _delta );
			world.Run<S_MovePlanets>( _delta );
			world.Run<S_MoveSpaceships>( _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingleton<PhysicsWorld>();
			world.Run<S_SynchronizeMotionStateFromTransform>();
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::s_physicsDelta );
			world.Run<S_SynchronizeTransformFromMotionState>();
			world.Run<S_MoveFollowTransforms>();
			world.Run<SMoveFollowTransformsUI>();

            world.Run<SHoverButtons>();
            world.Run<SHighlightButtons>();
			
			world.Run<S_FireWeapons>(			 _delta );
			world.Run<S_GenerateLightMesh>(		 _delta );
			world.Run<S_UpdateSolarPannels>(	 _delta );
			world.Run<S_RechargeBatteries>(		 _delta );
			world.Run<S_UpdateExpirationTimes>(	 _delta );
			world.Run<S_EruptionDamage>(		 _delta );
			world.Run<S_UpdateGameUiValues>(	 _delta );
			world.Run<S_UpdateGameUiPosition>(	 _delta );
			SolarEruption::Step(world,			 _delta );
			world.Run<S_PlayerDeath>(			 _delta );

			// late update
			const RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
			if( !renderWorld.isHeadless )
			{
				world.Run<S_ParticlesOcclusion>( _delta );
				world.Run<S_UpdateParticles>( _delta );
				world.Run<S_EmitParticles>( _delta );
				world.Run<S_GenerateParticles>( _delta );
				world.Run<S_UpdateBoundsFromRigidbody>( _delta );
				world.Run<S_UpdateBoundsFromModel>();
				world.Run<S_UpdateBoundsFromTransform>();
				world.Run<S_UpdateGameCamera>( _delta );
			}

			world.Run<S_ClientSaveState>(	_delta );
			world.Run<S_RollbackStateSave>( _delta );

			world.Run<S_ClientSend>(		_delta );
		}
	}

	//================================================================================================================================
	// Creates the joystick/keyboard axes in the input manager
	//================================================================================================================================
	void GameClient::CreateGameAxes()
	{
		Input::Get().Manager().CreateKeyboardAxis( "game_forward", Keyboard::W, Keyboard::S );
		Input::Get().Manager().CreateKeyboardAxis( "game_left", Keyboard::A, Keyboard::D );
		Input::Get().Manager().CreateKeyboardAxis( "game_fire", Keyboard::SPACE, Keyboard::NONE );
		Input::Get().Manager().CreateKeyboardAxis( "game_boost", Keyboard::LEFT_SHIFT, Keyboard::LEFT_CONTROL );
		Input::Get().Manager().CreateKeyboardAxis( "game_axis_stop", Keyboard::LEFT_CONTROL, Keyboard::NONE );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_x_axis_direction", 0, Joystick::RIGHT_X );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_y_axis_direction", 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_forward", 0, Joystick::LEFT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_axis_left", 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_axis_boost", 0, Joystick::RIGHT_Y );
		Input::Get().Manager().CreateJoystickAxis( "gamejs_axis_fire", 0, Joystick::RIGHT_TRIGGER );
		Input::Get().Manager().CreateJoystickButtons( "gamejs_axis_stop", 0, Joystick::A );
	}
}