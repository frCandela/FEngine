#include "game/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"

#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/systems/fanUpdateTimers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/components/fanCamera.hpp"
#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanHostReplication.hpp"
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
        EcsIncludeBase(mWorld);
        EcsIncludePhysics(mWorld);
        EcsIncludeRender3D(mWorld);
        EcsIncludeNetworkServer( mWorld );

		// game components
		mWorld.AddComponentType<Planet>();
		mWorld.AddComponentType<SpaceShip>();
		mWorld.AddComponentType<PlayerInput>();
		mWorld.AddComponentType<Weapon>();
		mWorld.AddComponentType<Bullet>();
		mWorld.AddComponentType<Battery>();
		mWorld.AddComponentType<SolarPanel>();
		mWorld.AddComponentType<Health>();
		mWorld.AddComponentType<Damage>();

		// game singleton components
		mWorld.AddSingletonType<SunLight>();
		mWorld.AddSingletonType<GameCamera>();
		mWorld.AddSingletonType<CollisionManager>();
		mWorld.AddSingletonType<Game>();
		mWorld.AddSingletonType<SolarEruption>();
		mWorld.AddSingletonType<ServerNetworkManager>();

        mWorld.AddTagType<TagSunlightOcclusion>();

        mName = "server";
        Game& game = mWorld.GetSingleton<Game>();
        game.mIsServer = true;
        mWorld.GetSingleton<Scene>().mOnEditorUseGameCamera.Connect( &GameServer::UseGameCamera, this );
	}

	//========================================================================================================
	//========================================================================================================
	void GameServer::Start()
	{
        ServerNetworkManager & netManager = mWorld.GetSingleton<ServerNetworkManager>();
		netManager.Start( mWorld );

        mWorld.Run<SRegisterAllRigidbodies>();
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
        mWorld.Run<SUnregisterAllRigidbodies>();

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

			SServerReceive::Run( mWorld, _delta );
            mWorld.Run<SProcessTimedOutPackets>	();
            mWorld.Run<SDetectHostTimout>		();
            mWorld.Run<SHostSpawnShip>			( _delta );
            mWorld.Run<SHostSyncFrame>			( _delta );
			SpawnManager			::Update( mWorld );

			// update	
			mWorld.Run<SHostUpdateInput>( _delta );
			mWorld.Run<SMovePlanets>	( _delta );
			mWorld.Run<SMoveSpaceships> ( _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
            mWorld.Run<SSynchronizeMotionStateFromTransform>();
			physicsWorld.mDynamicsWorld->stepSimulation( _delta, 10, Time::sPhysicsDelta );
			mWorld.Run<SSynchronizeTransformFromMotionState>();
			mWorld.Run<SMoveFollowTransforms>();

			mWorld.Run<SFireWeapons>( _delta );
			mWorld.Run<SGenerateLightMesh>( _delta );
			mWorld.Run<SUpdateSolarPanels>( _delta );
			mWorld.Run<SRechargeBatteries>( _delta );
			mWorld.Run<SUpdateExpirationTimes>( _delta );
			mWorld.Run<SEruptionDamage>( _delta );
			SolarEruption::Step( mWorld, _delta );
            mWorld.Run<SPlayerDeath>( _delta );

			// late update
			const RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
			if( ! renderWorld.mIsHeadless )
			{
				SCOPED_PROFILE( game_late );
				mWorld.Run<SUpdateBoundsFromRigidbody>( _delta );
				mWorld.Run<SUpdateBoundsFromModel>();
				mWorld.Run<SUpdateBoundsFromTransform>();
				mWorld.Run<SUpdateGameCamera>( _delta );
			}

			mWorld.Run<SHostSaveState>( _delta );
			mWorld.Run<SUpdateReplication>();
			mWorld.Run<SServerSend>( _delta );
		}
	}

    //========================================================================================================
    //========================================================================================================
    void GameServer::UpdateRenderWorld()
    {
        mWorld.ForceRun<SUpdateRenderWorldModels>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    //========================================================================================================
    //========================================================================================================
    void GameServer::UseGameCamera()
    {
        GameCamera& gameCamera = mWorld.GetSingleton<GameCamera>();
        Scene& scene = mWorld.GetSingleton<Scene>();
        scene.SetMainCamera( gameCamera.cmCameraHandle );
    }
}