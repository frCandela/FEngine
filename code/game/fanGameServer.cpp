#include "game/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"

#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/systems/fanUpdateTimers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/singletons/fanSceneResources.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/singletons/fanRenderDebug.hpp"

#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanHostPersistentHandle.hpp"
#include "network/components/fanEntityReplication.hpp"
#include "network/systems/fanServerUpdates.hpp"
#include "network/systems/fanServerSendReceive.hpp"
#include "network/systems/fanTimeout.hpp"

#include "game/fanGameTags.hpp"
#include "game/singletons/fanServerNetworkManager.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanSunLight.hpp"
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

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void GameServer::Init()
	{
        mName = "server";
        mOnSwitchToGameCamera.Connect( &GameServer::SwitchToGameCamera, this );

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
		// net components
		mWorld.AddComponentType<HostGameData>();
		mWorld.AddComponentType<HostConnection>();
		mWorld.AddComponentType<HostReplication>();
		mWorld.AddComponentType<ReliabilityLayer>();
		mWorld.AddComponentType<HostPersistentHandle>();
		mWorld.AddComponentType<EntityReplication>();
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
		mWorld.AddSingletonType<ServerNetworkManager>();
		// net singleton components
		mWorld.AddSingletonType<ServerConnection>();
		mWorld.AddSingletonType<LinkingContext>();
		mWorld.AddSingletonType<HostManager>();
		mWorld.AddSingletonType<Time>();

        mWorld.AddTagType<tag_sunlight_occlusion>();

        Game& game = mWorld.GetSingleton<Game>();
        game.mIsServer = true;
	}

	//========================================================================================================
	//========================================================================================================
	void GameServer::Start()
	{
        ServerNetworkManager & netManager = mWorld.GetSingleton<ServerNetworkManager>();
		netManager.Start( mWorld );

        mWorld.Run<S_RegisterAllRigidbodies>();
        GameCamera& gameCamera = GameCamera::CreateGameCamera( mWorld );
        Scene& scene = mWorld.GetSingleton<Scene>();
        scene.SetMainCamera( gameCamera.cmCameraHandle );

		SolarEruption::Start( mWorld );

		SolarEruption& eruption = mWorld.GetSingleton<SolarEruption>();
		eruption.ScheduleNextEruption( mWorld );

        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        SunLight& sunLight = mWorld.GetSingleton<SunLight>();
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( sunLight.mMesh, "sunlight_mesh_" + mName );
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );
	}

	//========================================================================================================
	//========================================================================================================
	void  GameServer::Stop()
	{
		// clears the physics world
        mWorld.Run<S_UnregisterAllRigidbodies>();

		// clears the registered mesh
		RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        SunLight& sunLight = mWorld.GetSingleton<SunLight>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        meshManager.Remove( sunLight.mMesh->mPath );
        meshManager.Remove( renderWorld.mParticlesMesh->mPath );

		GameCamera::DeleteGameCamera( mWorld );
        ServerNetworkManager & netManager = mWorld.GetSingleton<ServerNetworkManager>();
		netManager.Stop( mWorld );
	}

	//========================================================================================================
	//========================================================================================================
	void  GameServer::Step( const float _delta )
	{		
		Time& time = mWorld.GetSingleton<Time>();

		if( _delta > 0.f )
		{
			time.mFrameIndex++;
		}

		{
			SCOPED_PROFILE( scene_update );

			S_ServerReceive::Run( mWorld, _delta );
            mWorld.Run<S_ProcessTimedOutPackets>	();
            mWorld.Run<S_DetectHostTimout>		();
            mWorld.Run<S_HostSpawnShip>			( _delta );
            mWorld.Run<S_HostSyncFrame>			( _delta );
			SpawnManager			::Update( mWorld );

			// update	
			mWorld.Run<S_HostUpdateInput>( _delta );
			mWorld.Run<SMovePlanets>	( _delta );
			mWorld.Run<SMoveSpaceships> ( _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
            mWorld.Run<S_SynchronizeMotionStateFromTransform>();
			physicsWorld.mDynamicsWorld->stepSimulation( _delta, 10, Time::sPhysicsDelta );
			mWorld.Run<S_SynchronizeTransformFromMotionState>();
			mWorld.Run<SMoveFollowTransforms>();

			mWorld.Run<SFireWeapons>( _delta );
			mWorld.Run<SGenerateLightMesh>( _delta );
			mWorld.Run<SUpdateSolarPanels>( _delta );
			mWorld.Run<SRechargeBatteries>( _delta );
			mWorld.Run<S_UpdateExpirationTimes>(	_delta );
			mWorld.Run<SEruptionDamage>( _delta );
			mWorld.Run<SUpdateGameUiValues>( _delta );
			mWorld.Run<SUpdateGameUiPosition>( _delta );
			SolarEruption::Step( mWorld,			_delta );
            mWorld.Run<SPlayerDeath>( _delta );

			// late update
			const RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
			if( ! renderWorld.mIsHeadless )
			{
				SCOPED_PROFILE( game_late );
				mWorld.Run<SParticlesOcclusion>( _delta );
				mWorld.Run<S_UpdateParticles>(			_delta );
				mWorld.Run<S_EmitParticles>(				_delta );
				mWorld.Run<S_GenerateParticles>(			_delta );
				mWorld.Run<S_UpdateBoundsFromRigidbody>( _delta );
				mWorld.Run<S_UpdateBoundsFromModel>();
				mWorld.Run<S_UpdateBoundsFromTransform>();
				mWorld.Run<SUpdateGameCamera>( _delta );
			}

			mWorld.Run<S_HostSaveState>( _delta );
			mWorld.Run<S_UpdateReplication>();
			mWorld.Run<S_ServerSend>( _delta );
		}
	}

    //========================================================================================================
    //========================================================================================================
    void GameServer::SwitchToGameCamera()
    {
        GameCamera& gameCamera = mWorld.GetSingleton<GameCamera>();
        Scene& scene = mWorld.GetSingleton<Scene>();
        scene.SetMainCamera( gameCamera.cmCameraHandle );
    }
}