#include "fanGameClient.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "network/singletons/fanTime.hpp"

#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/systems/fanSynchronizeMotionStates.hpp"
#include "engine/systems/fanRegisterPhysics.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanRaycastUI.hpp"
#include "engine/systems/fanUpdateUIAlign.hpp"
#include "engine/systems/fanUpdateUILayouts.hpp"
#include "engine/systems/fanUpdateUIText.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/singletons/fanScene.hpp"

#include "network/fanPacket.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/systems/fanClientUpdates.hpp"
#include "network/systems/fanClientSendReceive.hpp"
#include "network/systems/fanTimeout.hpp"
#include "network/systems/fanRollback.hpp"

#include "game/fanGameTags.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanDamage.hpp"
#include "game/singletons/ui/fanUIMainMenu.hpp"
#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"
#include "game/systems/fanUpdateGameCamera.hpp"
#include "game/systems/fanUpdateWeapons.hpp"
#include "game/systems/fanUpdatePlayerInput.hpp"
#include "game/systems/fanUpdateEnergy.hpp"
#include "game/systems/fanUpdateGameUI.hpp"
#include "game/systems/fanParticlesOcclusion.hpp"
#include "game/spawn/fanRegisterSpawnMethods.hpp"

#ifdef FAN_EDITOR
#include "editor/fanRegisterEditorGui.hpp"
#endif

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void GameClient::Init()
	{
        EcsIncludeEngine( mWorld );
        EcsIncludePhysics(mWorld);
        EcsIncludeRender3D(mWorld);
        EcsIncludeRenderUI(mWorld);
        EcsIncludeNetworkClient( mWorld );

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
		// game singletons
		mWorld.AddSingletonType<SunLight>();
		mWorld.AddSingletonType<GameCamera>();
		mWorld.AddSingletonType<CollisionManager>();
		mWorld.AddSingletonType<Game>();
		mWorld.AddSingletonType<SolarEruption>();
		mWorld.AddSingletonType<SpawnManager>();
		mWorld.AddSingletonType<ClientNetworkManager>();
        mWorld.AddSingletonType<UIMainMenu>();
        // game tags
		mWorld.AddTagType<TagSunlightOcclusion>();

        mName = "client";
        mWorld.GetSingleton<Scene>().mOnEditorUseProjectCamera.Connect( &GameClient::UseGameCamera, this );
        mWorld.GetSingleton<Scene>().mOnLoad.Connect( &GameClient::OnLoadScene, this );

        RegisterGameSpawnMethods( mWorld.GetSingleton<SpawnManager>() );
#ifdef FAN_EDITOR
        RegisterEditorGuiInfos( mWorld.GetSingleton< EditorGuiInfo >() );
#endif

        CreateGameAxes();
	}

	//========================================================================================================
	//========================================================================================================
	void GameClient::Start()
	{
		ClientNetworkManager& netManager = mWorld.GetSingleton<ClientNetworkManager>();
		netManager.Start( mWorld );

		// Init game
        mWorld.Run<SRegisterAllRigidbodies>();
        GameCamera& gameCamera = GameCamera::CreateGameCamera( mWorld );
        Scene& scene = mWorld.GetSingleton<Scene>();
        scene.SetMainCamera( gameCamera.mCameraHandle );

		SolarEruption::Start( mWorld );

        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        SunLight& sunLight = mWorld.GetSingleton<SunLight>();
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( sunLight.mMesh, "sunlight_mesh_" + mName );
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );

        UIMainMenu& mainMenu = mWorld.GetSingleton<UIMainMenu>();
        UIMainMenu::ShowMainMenu( mainMenu );
	}

	//========================================================================================================
	//========================================================================================================
	void  GameClient::Stop()
	{
		// clears the physics world
        mWorld.Run<SUnregisterAllRigidbodies>();

		// clears the particles mesh
		RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        SunLight& sunLight = mWorld.GetSingleton<SunLight>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        meshManager.Remove( sunLight.mMesh->mPath );
        meshManager.Remove( renderWorld.mParticlesMesh->mPath );

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
		const EcsEntity persistentID = mWorld.GetEntity( netManager.mPersistentHandle );
		ClientGameData& gameData = mWorld.GetComponent<ClientGameData>( persistentID );
		if( !gameData.mSpaceshipSynced )
		{
			Time& time = mWorld.GetSingleton<Time>();
			const FrameIndex lastFrame = time.mFrameIndex;
			const FrameIndex firstFrame = gameData.mLastServerState.mFrameIndex;
            mWorld.Run<SRollbackInit>();
			Debug::Highlight() << "rollback to frame " << firstFrame << Debug::Endl();

			// Rollback at the frame we took the snapshot of the player game state
			time.mFrameIndex = firstFrame;

			// reset world to first frame		
			PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
			physicsWorld.Reset();
            mWorld.Run<SRollbackRestoreState>( firstFrame );
			const EcsEntity spaceshipID = mWorld.GetEntity( gameData.sSpaceshipHandle );
			Rigidbody& rigidbody = mWorld.GetComponent<Rigidbody>( spaceshipID );
			physicsWorld.mDynamicsWorld->removeRigidBody( rigidbody.mRigidbody );
			physicsWorld.mDynamicsWorld->addRigidBody( rigidbody.mRigidbody );
			rigidbody.ClearForces();
			Transform& transform = mWorld.GetComponent<Transform>( spaceshipID );
			rigidbody.SetVelocity( gameData.mLastServerState.mVelocity );
			rigidbody.SetAngularVelocity( gameData.mLastServerState.mAngularVelocity );
			transform.SetPosition( gameData.mLastServerState.mPosition );
			transform.SetRotationEuler( gameData.mLastServerState.mOrientation );

			// Clear previous states & saves the last correct server state
			gameData.mPreviousLocalStates = std::queue<PacketPlayerGameState >();
			gameData.mPreviousLocalStates.push( gameData.mLastServerState );

			// resimulate the last frames of input of the player
			const float delta = time.mLogicDelta;
			while( time.mFrameIndex < lastFrame )
			{
				time.mFrameIndex++;

                mWorld.Run<SRollbackRestoreState>( time.mFrameIndex );

                mWorld.Run<SMovePlanets>( delta );
                mWorld.Run<SMoveSpaceships>( delta );

                mWorld.Run<SSynchronizeMotionStateFromTransform>();
				physicsWorld.mDynamicsWorld->stepSimulation( time.mLogicDelta, 10, Time::sPhysicsDelta );
                mWorld.Run<SSynchronizeTransformFromMotionState>();

                mWorld.Run<SClientSaveState>( delta );
                mWorld.Run<SRollbackStateSave>( delta );
			}

			gameData.mSpaceshipSynced = true;
            fanAssert( time.mFrameIndex == lastFrame );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void  GameClient::Step( const float _delta )
	{		
		Time& time = mWorld.GetSingleton<Time>();

		{
			SCOPED_PROFILE( scene_update );
            mWorld.Run<SClientReceive>( _delta );
            mWorld.Run<SRollbackRemoveOldStates>( _delta );

			RollbackResimulate( _delta );

			if( _delta > 0.f )
			{
				time.mFrameIndex++;
			}

            mWorld.Run<SProcessTimedOutPackets>();
            mWorld.Run<SClientDetectServerTimeout>( _delta );
            mWorld.Run<SClientRunReplication>( _delta );
			SpawnManager::Update( mWorld );

			// update
            mWorld.Run<SRefreshPlayerInput>( _delta );
            mWorld.Run<SClientSaveInput>( _delta );
            mWorld.Run<SMovePlanets>( _delta );
            mWorld.Run<SMoveSpaceships>( _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
            mWorld.Run<SSynchronizeMotionStateFromTransform>();
			physicsWorld.mDynamicsWorld->stepSimulation( _delta, 10, Time::sPhysicsDelta );
            mWorld.Run<SSynchronizeTransformFromMotionState>();
            mWorld.Run<SMoveFollowTransforms>();

            mWorld.Run<SUpdateUIText>();
            mWorld.Run<SAlignUI>();
            mWorld.Run<SUpdateUILayouts>();
            mWorld.Run<SHoverButtons>();
            mWorld.Run<SHighlightButtons>();

			mWorld.Run<SFireWeapons>( _delta );
			mWorld.Run<SGenerateLightMesh>( _delta );
			mWorld.Run<SUpdateSolarPanels>( _delta );
			mWorld.Run<SRechargeBatteries>( _delta );
			mWorld.Run<SUpdateExpirationTimes>( _delta );
			mWorld.Run<SEruptionDamage>( _delta );
			mWorld.Run<SUpdateGameUiValues>( _delta );
			mWorld.Run<SUpdateGameUiPosition>( _delta );
			SolarEruption::Step( mWorld,			 _delta );
            mWorld.Run<SPlayerDeath>( _delta );

			// late update
			const RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
			if( !renderWorld.mIsHeadless )
			{
				mWorld.Run<SParticlesOcclusion>( _delta );
				mWorld.Run<SUpdateParticles>( _delta );
				mWorld.Run<SEmitParticles>( _delta );
				mWorld.Run<SGenerateParticles>( _delta );
				mWorld.Run<SUpdateBoundsFromRigidbody>( _delta );
				mWorld.Run<SUpdateBoundsFromModel>();
				mWorld.Run<SUpdateBoundsFromTransform>();
				mWorld.Run<SUpdateGameCamera>( _delta );
			}

            mWorld.Run<SClientSaveState>( _delta );
            mWorld.Run<SRollbackStateSave>( _delta );

            mWorld.Run<SClientSend>( _delta );
		}
	}

    //========================================================================================================
    //========================================================================================================
    void GameClient::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );
        mWorld.ForceRun<SUpdateRenderWorldModels>();
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
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

    //========================================================================================================
    //========================================================================================================
    void GameClient::UseGameCamera()
	{
        GameCamera& gameCamera = mWorld.GetSingleton<GameCamera>();
        Scene& scene = mWorld.GetSingleton<Scene>();
        scene.SetMainCamera( gameCamera.mCameraHandle );
	}

    //========================================================================================================
    //========================================================================================================
    void GameClient::OnLoadScene( Scene& _scene )
	{
        (void) _scene;

        UIMainMenu& mainMenu = mWorld.GetSingleton<UIMainMenu>();
        UIMainMenu::ShowMainMenu( mainMenu );
	}
}