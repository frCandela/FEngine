#include "project_spaceships/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"

#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/systems/fanSynchronizeMotionStates.hpp"
#include "engine/systems/fanRegisterPhysics.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanRenderResources.hpp"

#include "network/components/fanHostReplication.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/systems/fanServerUpdates.hpp"
#include "network/systems/fanServerSendReceive.hpp"
#include "network/systems/fanTimeout.hpp"

#include "project_spaceships/editor/fanRegisterEditorGui.hpp"
#include "project_spaceships/fanGameTags.hpp"
#include "project_spaceships/systems/fanUpdatePlanets.hpp"
#include "project_spaceships/systems/fanUpdateSpaceships.hpp"
#include "project_spaceships/systems/fanUpdateGameCamera.hpp"
#include "project_spaceships/systems/fanUpdateWeapons.hpp"
#include "project_spaceships/systems/fanUpdatePlayerInput.hpp"
#include "project_spaceships/systems/fanUpdateEnergy.hpp"
#include "project_spaceships/systems/fanUpdateGameUI.hpp"
#include "project_spaceships/systems/fanParticlesOcclusion.hpp"
#include "project_spaceships/spawn/fanRegisterSpawnMethods.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void GameServer::Init()
	{
        EcsIncludeEngine( mWorld );
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
        mWorld.GetSingleton<Scene>().mOnEditorUseProjectCamera.Connect( &GameServer::UseGameCamera, this );

        RegisterGameSpawnMethods( mWorld.GetSingleton<SpawnManager>() );
        RegisterEditorGuiInfos( mWorld.GetSingleton< EditorGuiInfo >() );
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
        scene.SetMainCamera( gameCamera.mCameraHandle );

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
        scene.SetMainCamera( gameCamera.mCameraHandle );
    }
}