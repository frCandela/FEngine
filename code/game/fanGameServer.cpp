#include "game/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"
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
#include "scene/fanSceneTags.hpp"
#include "game/fanGameTags.hpp"

#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanHostManager.hpp"

#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/systems/fanServerUpdates.hpp"
#include "network/systems/fanServerSendReceive.hpp"
#include "network/systems/fanTimeout.hpp"

#include "game/singletons/fanServerNetworkManager.hpp"
#include "game/singletons/fanCollisionManager.hpp"
#include "game/singletons/fanSolarEruption.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanSunLight.hpp"
#include "game/singletons/fanGame.hpp"
#include "network/singletons/fanSpawnManager.hpp"

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

		// base singleton components
		world.AddSingletonType<Scene>();
		world.AddSingletonType<RenderWorld>();
		world.AddSingletonType<PhysicsWorld>();
		world.AddSingletonType<ScenePointers>();
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
		
		world.AddTagType<tag_boundsOutdated>();
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
		S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Stop()
	{
		// clears the physics world
		S_UnregisterAllRigidbodies::Run( world, world.Match( S_UnregisterAllRigidbodies::GetSignature( world ) ) );

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
	void GameServer::Pause()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Resume()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Step( const float _delta )
	{
		if( _delta > 0.f )
		{
			game->frameIndex++;
		}

		{
			SCOPED_PROFILE( scene_update );

			S_ServerReceive			::Run( world, _delta );
			S_ProcessTimedOutPackets::Run( world, world.Match( S_ProcessTimedOutPackets::GetSignature( world ) ) );
			S_DetectHostTimout		::Run( world, world.Match( S_DetectHostTimout::GetSignature( world ) ) );
			S_HostSpawnShip			::Run( world, world.Match( S_HostSpawnShip::GetSignature( world ) ), _delta );
			S_HostSyncFrame			::Run( world, world.Match( S_HostSyncFrame::GetSignature( world ) ), _delta );
			SpawnManager			::Spawn( world );

			// update	
			S_HostUpdateInput::Run( world, world.Match( S_HostUpdateInput::GetSignature( world ) ), _delta );
			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), _delta );
			S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingleton<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform	::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ) );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState	::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ) );
			S_MoveFollowTransforms					::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI				::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );			
			
			S_HostSaveState		::Run( world, world.Match( S_HostSaveState::GetSignature( world ) )		, _delta );			
			S_FireWeapons::Run( world, world.Match( S_FireWeapons::GetSignature( world ) ), _delta );		
			S_GenerateLightMesh		::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) )		, _delta );
			S_UpdateSolarPannels	::Run( world, world.Match( S_UpdateSolarPannels::GetSignature( world ) )	, _delta );
			S_RechargeBatteries		::Run( world, world.Match( S_RechargeBatteries::GetSignature( world ) )		, _delta );
			S_UpdateExpirationTimes	::Run( world, world.Match( S_UpdateExpirationTimes::GetSignature( world ) )	, _delta );
			S_EruptionDamage		::Run( world, world.Match( S_EruptionDamage::GetSignature( world ) )		, _delta );
			S_UpdateGameUiValues	::Run( world, world.Match( S_UpdateGameUiValues::GetSignature( world ) )	, _delta );
			S_UpdateGameUiPosition	::Run( world, world.Match( S_UpdateGameUiPosition::GetSignature( world ) )	, _delta );
			SolarEruption			::Step( world																, _delta );
			S_PlayerDeath			::Run( world, world.Match( S_PlayerDeath::GetSignature( world ) )			, _delta );

			// late update
			{
				SCOPED_PROFILE( game_late );

				S_ParticlesOcclusion::Run( world, world.Match( S_ParticlesOcclusion::GetSignature( world ) ), _delta );
				S_UpdateParticles::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) ), _delta );
				S_EmitParticles::Run( world, world.Match( S_EmitParticles::GetSignature( world ) ), _delta );
				S_GenerateParticles::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) ), _delta );
				S_UpdateBoundsFromRigidbody::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) ), _delta );
				S_UpdateBoundsFromModel::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) ) );
				S_UpdateBoundsFromTransform::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) ) );
				S_UpdateGameCamera::Run( world, world.Match( S_UpdateGameCamera::GetSignature( world ) ), _delta );
			}

			HostManager& hostManager = world.GetSingleton<HostManager>();
			for( const std::pair<HostManager::IPPort, EcsHandle>& pair : hostManager.hostHandles )
			{
				const EcsHandle hostHandle = pair.second;
				const EcsEntity hostID = world.GetEntity( hostHandle );
				const HostGameData& hostData = world.GetComponent<HostGameData>( hostID );
				if( hostData.spaceshipHandle != 0 )
				{
					const PacketReplication packet = HostReplication::BuildEntityPacket( world, hostData.spaceshipHandle, { Transform::Info::s_type/*,Rigidbody::s_typeInfo*/ } );

					for( const std::pair<HostManager::IPPort, EcsHandle>& otherPair : hostManager.hostHandles )
					{
						const EcsHandle otherHostHandle = otherPair.second;
						if( otherHostHandle != hostHandle )
						{
							const EcsEntity otherHostID = world.GetEntity( otherHostHandle );
							HostReplication& hostReplication = world.GetComponent<HostReplication>( otherHostID );
							hostReplication.Replicate( packet, HostReplication::None );
						}
					}
				}
			}

			S_ServerSend				::Run( world, world.Match( S_ServerSend::GetSignature( world ) )				, _delta );
		}
	}
}