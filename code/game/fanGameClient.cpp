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
#include "scene/systems/fanUpdateUIAlign.hpp"
#include "scene/systems/fanUpdateUILayouts.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
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
	//========================================================================================================
	//========================================================================================================
	void GameClient::Init()
	{
        mName = "client";

		// base components
		mWorld.AddComponentType<SceneNode>();
		mWorld.AddComponentType<Transform>();
		mWorld.AddComponentType<DirectionalLight>();
		mWorld.AddComponentType<PointLight>();
		mWorld.AddComponentType<MeshRenderer>();
		mWorld.AddComponentType<Material>();
		mWorld.AddComponentType<Camera>();
		mWorld.AddComponentType<ParticleEmitter>();
		mWorld.AddComponentType<Particle>();
		mWorld.AddComponentType<Rigidbody>();
		mWorld.AddComponentType<MotionState>();
		mWorld.AddComponentType<BoxShape>();
		mWorld.AddComponentType<SphereShape>();
		mWorld.AddComponentType<UITransform>();
		mWorld.AddComponentType<UIRenderer>();
		mWorld.AddComponentType<Bounds>();
		mWorld.AddComponentType<ExpirationTime>();
		mWorld.AddComponentType<FollowTransform>();
		mWorld.AddComponentType<UIProgressBar>();
		mWorld.AddComponentType<UIButton>();
        mWorld.AddComponentType<UILayout>();
        mWorld.AddComponentType<UIAlign>();
		// game components
		mWorld.AddComponentType<Planet>();
		mWorld.AddComponentType<SpaceShip>();
		mWorld.AddComponentType<PlayerInput>();
		mWorld.AddComponentType<Weapon>();
		mWorld.AddComponentType<Bullet>();
		mWorld.AddComponentType<Battery>();
		mWorld.AddComponentType<SolarPanel>();
		mWorld.AddComponentType<Health>();
		mWorld.AddComponentType<SpaceshipUI>();
		mWorld.AddComponentType<Damage>();
		mWorld.AddComponentType<PlayerController>();
        mWorld.AddComponentType<UIMainMenu>();
		// network components
		mWorld.AddComponentType<ReliabilityLayer>();
		mWorld.AddComponentType<ClientConnection>();
		mWorld.AddComponentType<ClientReplication>();
		mWorld.AddComponentType<ClientRPC>();
		mWorld.AddComponentType<ClientGameData>();
		mWorld.AddComponentType<ClientRollback>();
		mWorld.AddComponentType<LinkingContextUnregisterer>();

		// base singleton components
		mWorld.AddSingletonType<Scene>();
        mWorld.AddSingletonType<RenderResources>();
        mWorld.AddSingletonType<SceneResources>();
		mWorld.AddSingletonType<RenderWorld>();
		mWorld.AddSingletonType<PhysicsWorld>();
		mWorld.AddSingletonType<ScenePointers>();
		mWorld.AddSingletonType<RenderDebug>();
		mWorld.AddSingletonType<Mouse>();
		// game singleton components
		mWorld.AddSingletonType<SunLight>();
		mWorld.AddSingletonType<GameCamera>();
		mWorld.AddSingletonType<CollisionManager>();
		mWorld.AddSingletonType<Game>();
		mWorld.AddSingletonType<SolarEruption>();
		mWorld.AddSingletonType<SpawnManager>();
		mWorld.AddSingletonType<ClientNetworkManager>();
		// network singleton components
		mWorld.AddSingletonType<LinkingContext>();
		mWorld.AddSingletonType<Time>();

		mWorld.AddTagType<tag_sunlight_occlusion>();
	}

	//========================================================================================================
	//========================================================================================================
	void GameClient::Start()
	{
		ClientNetworkManager& netManager = mWorld.GetSingleton<ClientNetworkManager>();
		netManager.Start( mWorld );

		// Init game
        mWorld.Run<S_RegisterAllRigidbodies>();
		GameCamera::CreateGameCamera( mWorld );
		SolarEruption::Start( mWorld );

        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        SunLight& sunLight = mWorld.GetSingleton<SunLight>();
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( sunLight.mesh, "sunlight_mesh_" + mName );
        meshManager.Add( renderWorld.particlesMesh, "particles_mesh_" + mName );
	}

	//========================================================================================================
	//========================================================================================================
	void  GameClient::Stop()
	{
		// clears the physics world
        mWorld.Run<S_UnregisterAllRigidbodies>();

		// clears the particles mesh
		RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        SunLight& sunLight = mWorld.GetSingleton<SunLight>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        meshManager.Remove( sunLight.mesh->mPath );
        meshManager.Remove( renderWorld.particlesMesh->mPath );

		GameCamera::DeleteGameCamera( mWorld );

		ClientNetworkManager& netManager = mWorld.GetSingleton<ClientNetworkManager>();
		netManager.Stop( mWorld );
	}

	//========================================================================================================
	//========================================================================================================
	void GameClient::RollbackResimulate( const float _delta )
	{
	    if( _delta == 0.f ) { return; }

		ClientNetworkManager& netManager = mWorld.GetSingleton<ClientNetworkManager>();
		const EcsEntity persistentID = mWorld.GetEntity( netManager.persistentHandle );
		ClientGameData& gameData = mWorld.GetComponent<ClientGameData>( persistentID );
		if( !gameData.spaceshipSynced )
		{
			Time& time = mWorld.GetSingleton<Time>();
			const FrameIndex lastFrame = time.frameIndex;
			const FrameIndex firstFrame = gameData.lastServerState.frameIndex;
            mWorld.Run<S_RollbackInit>();
			Debug::Highlight() << "rollback to frame " << firstFrame << Debug::Endl();

			// Rollback at the frame we took the snapshot of the player game state
			time.frameIndex = firstFrame;

			// reset world to first frame		
			PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
			physicsWorld.Reset();
            mWorld.Run<S_RollbackRestoreState>( firstFrame );
			const EcsEntity spaceshipID = mWorld.GetEntity( gameData.spaceshipHandle );
			Rigidbody& rigidbody = mWorld.GetComponent<Rigidbody>( spaceshipID );
			physicsWorld.dynamicsWorld->removeRigidBody( rigidbody.rigidbody );
			physicsWorld.dynamicsWorld->addRigidBody( rigidbody.rigidbody );
			rigidbody.ClearForces();
			Transform& transform = mWorld.GetComponent<Transform>( spaceshipID );
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

                mWorld.Run<S_RollbackRestoreState>( time.frameIndex );

                mWorld.Run<S_MovePlanets>( delta );
                mWorld.Run<S_MoveSpaceships>( delta );

                mWorld.Run<S_SynchronizeMotionStateFromTransform>();
				physicsWorld.dynamicsWorld->stepSimulation( time.logicDelta, 10, Time::s_physicsDelta );
                mWorld.Run<S_SynchronizeTransformFromMotionState>();

                mWorld.Run<S_ClientSaveState>( delta );
                mWorld.Run<S_RollbackStateSave>( delta );
			}

			gameData.spaceshipSynced = true;
			assert( time.frameIndex == lastFrame );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void  GameClient::Step( const float _delta )
	{		
		Time& time = mWorld.GetSingleton<Time>();

		{
			SCOPED_PROFILE( scene_update );
            mWorld.Run<S_ClientReceive>( _delta );
            mWorld.Run<S_RollbackRemoveOldStates>( _delta );

			RollbackResimulate( _delta );

			if( _delta > 0.f )
			{
				time.frameIndex++;
			}

            mWorld.Run<S_ProcessTimedOutPackets>();
            mWorld.Run<S_ClientDetectServerTimeout>( _delta );
            mWorld.Run<S_ClientRunReplication>(		_delta );
			SpawnManager::Update( mWorld );

			// update
            mWorld.Run<S_RefreshPlayerInput>( _delta );
            mWorld.Run<S_ClientSaveInput>( _delta );
            mWorld.Run<S_MovePlanets>( _delta );
            mWorld.Run<S_MoveSpaceships>( _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
            mWorld.Run<S_SynchronizeMotionStateFromTransform>();
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::s_physicsDelta );
            mWorld.Run<S_SynchronizeTransformFromMotionState>();
            mWorld.Run<SMoveFollowTransforms>();

            mWorld.Run<SAlignUI>();
            mWorld.Run<SUpdateUILayouts>();
            mWorld.Run<SHoverButtons>();
            mWorld.Run<SHighlightButtons>();
			
			mWorld.Run<S_FireWeapons>(			 _delta );
			mWorld.Run<S_GenerateLightMesh>(		 _delta );
			mWorld.Run<S_UpdateSolarPannels>(	 _delta );
			mWorld.Run<S_RechargeBatteries>(		 _delta );
			mWorld.Run<S_UpdateExpirationTimes>(	 _delta );
			mWorld.Run<S_EruptionDamage>(		 _delta );
			mWorld.Run<S_UpdateGameUiValues>(	 _delta );
			mWorld.Run<S_UpdateGameUiPosition>(	 _delta );
			SolarEruption::Step( mWorld,			 _delta );
            mWorld.Run<S_PlayerDeath>(			 _delta );

			// late update
			const RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
			if( !renderWorld.isHeadless )
			{
				mWorld.Run<S_ParticlesOcclusion>( _delta );
				mWorld.Run<S_UpdateParticles>( _delta );
				mWorld.Run<S_EmitParticles>( _delta );
				mWorld.Run<S_GenerateParticles>( _delta );
				mWorld.Run<S_UpdateBoundsFromRigidbody>( _delta );
				mWorld.Run<S_UpdateBoundsFromModel>();
				mWorld.Run<S_UpdateBoundsFromTransform>();
				mWorld.Run<S_UpdateGameCamera>( _delta );
			}

            mWorld.Run<S_ClientSaveState>(	_delta );
            mWorld.Run<S_RollbackStateSave>( _delta );

            mWorld.Run<S_ClientSend>(		_delta );
		}
	}

	//========================================================================================================
	// Creates the joystick/keyboard axes in the input manager
	//========================================================================================================
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