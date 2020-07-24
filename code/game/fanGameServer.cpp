#include "game/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/fanPacket.hpp"

#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/systems/fanUpdateTimers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanExpirationTime.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/fanSceneTags.hpp"

#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/singletons/fanTime.hpp"
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
#include "game/singletons/fanCollisionManager.hpp"
#include "game/singletons/fanSolarEruption.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanSunLight.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"
#include "game/systems/fanUpdateGameCamera.hpp"
#include "game/systems/fanUpdateWeapons.hpp"
#include "game/systems/fanUpdatePlayerInput.hpp"
#include "game/systems/fanUpdateEnergy.hpp"
#include "game/systems/fanUpdateGameUI.hpp"
#include "game/systems/fanParticlesOcclusion.hpp"
#include "game/components/fanPlanet.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"
#include "game/components/fanSpaceshipUI.hpp"
#include "game/components/fanDamage.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameServer::GameServer( const std::string _name )
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
		world.AddComponentType<TransformUI>();
		world.AddComponentType<UIRenderer>();
		world.AddComponentType<Bounds>();
		world.AddComponentType<ExpirationTime>();
		world.AddComponentType<FollowTransform>();
		world.AddComponentType<ProgressBar>();
		world.AddComponentType<FollowTransformUI>();
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
		// net components
		world.AddComponentType<HostGameData>();
		world.AddComponentType<HostConnection>();
		world.AddComponentType<HostReplication>();
		world.AddComponentType<ReliabilityLayer>();
		world.AddComponentType<HostPersistentHandle>();
		world.AddComponentType<EntityReplication>();
		world.AddComponentType<LinkingContextUnregisterer>();

		// base singleton components
		world.AddSingletonType<Scene>();
		world.AddSingletonType<RenderWorld>();
		world.AddSingletonType<PhysicsWorld>();
		world.AddSingletonType<ScenePointers>();
		world.AddSingletonType<RenderDebug>();
		// game singleton components
		world.AddSingletonType<SunLight>();
		world.AddSingletonType<GameCamera>();
		world.AddSingletonType<CollisionManager>();
		world.AddSingletonType<Game>();
		world.AddSingletonType<SolarEruption>();
		world.AddSingletonType<SpawnManager>();
		world.AddSingletonType<ServerNetworkManager>();
		// net singleton components
		world.AddSingletonType<ServerConnection>();
		world.AddSingletonType<LinkingContext>();
		world.AddSingletonType<HostManager>();
		world.AddSingletonType<Time>();
		
		world.AddTagType<tag_sunlight_occlusion>();

		// @hack
		Game& gameNotConst = world.GetSingleton<Game>();
		gameNotConst.gameServer = this;
		gameNotConst.name = _name;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Start()
	{
		game = &world.GetSingleton<Game>();
		netManager = &world.GetSingleton<ServerNetworkManager>();

		netManager->Start( world );
		world.Run<S_RegisterAllRigidbodies>();
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );	

		SolarEruption& eruption = world.GetSingleton<SolarEruption>();
		eruption.ScheduleNextEruption( world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Stop()
	{
		// clears the physics world
		world.Run<S_UnregisterAllRigidbodies>();

		// clears the particles mesh
		RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
		renderWorld.particlesMesh.LoadFromVertices( {} );

		GameCamera::DeleteGameCamera( world );

		netManager->Stop( world );

		game = nullptr;
		netManager = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Step( const float _delta )
	{		
		Time& time = world.GetSingleton<Time>();

		if( _delta > 0.f )
		{
			time.frameIndex++;
		}

		{
			SCOPED_PROFILE( scene_update );

			S_ServerReceive::Run( world, _delta );
			world.Run<S_ProcessTimedOutPackets>	();
			world.Run<S_DetectHostTimout>		();
			world.Run<S_HostSpawnShip>			( _delta );
			world.Run<S_HostSyncFrame>			( _delta );
			SpawnManager			::Update( world );

			// update	
			world.Run<S_HostUpdateInput>( _delta );
			world.Run<S_MovePlanets>	( _delta );
			world.Run<S_MoveSpaceships> ( _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingleton<PhysicsWorld>();
			world.Run<S_SynchronizeMotionStateFromTransform>();
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::s_physicsDelta );
			world.Run<S_SynchronizeTransformFromMotionState>();
			world.Run<S_MoveFollowTransforms>();
			world.Run<S_MoveFollowTransformsUI>();			

			world.Run<S_FireWeapons>(			_delta );
			//world.Run<S_GenerateLightMesh>(		_delta );
			world.Run<S_UpdateSolarPannels>(	_delta );
			world.Run<S_RechargeBatteries>(		_delta );
			world.Run<S_UpdateExpirationTimes>(	_delta );
			world.Run<S_EruptionDamage>(		_delta );
			world.Run<S_UpdateGameUiValues>(	_delta );
			world.Run<S_UpdateGameUiPosition>(	_delta );
			SolarEruption::Step( world,			_delta );
			world.Run<S_PlayerDeath>(			_delta );

			// late update
			const RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
			if( ! renderWorld.isHeadless )
			{
				SCOPED_PROFILE( game_late );
				world.Run<S_ParticlesOcclusion>(		_delta );
				world.Run<S_UpdateParticles>(			_delta );
				world.Run<S_EmitParticles>(				_delta );
				//world.Run<S_GenerateParticles>(			_delta );
				world.Run<S_UpdateBoundsFromRigidbody>( _delta );
				world.Run<S_UpdateBoundsFromModel>();
				world.Run<S_UpdateBoundsFromTransform>();
				world.Run<S_UpdateGameCamera>(			_delta );
			}

			world.Run<S_HostSaveState>( _delta );
			world.Run<S_UpdateReplication>();
			world.Run<S_ServerSend>( _delta );
		}
	}
}