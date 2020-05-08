#include "game/fanGameClient.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"

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
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "scene/singletonComponents/fanScenePointers.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/fanSceneTags.hpp"
#include "network/fanPacket.hpp"
#include "game/fanGameTags.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanClientConnection.hpp"

#include "game/singletonComponents/fanClientNetworkManager.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "game/singletonComponents/fanSolarEruption.hpp"
#include "game/singletonComponents/fanGame.hpp"

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
#include "game/components/fanPlayerController.hpp"

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
		world.AddComponentType<PlayerController>();
		// network components
		world.AddComponentType<ReliabilityLayer>();
		world.AddComponentType<ClientConnection>();
		world.AddComponentType<ClientReplication>();
		world.AddComponentType<ClientRPC>();

		// base singleton components
		world.AddSingletonComponentType<Scene>();
		world.AddSingletonComponentType<RenderWorld>();
		world.AddSingletonComponentType<PhysicsWorld>();
		world.AddSingletonComponentType<ScenePointers>();
		// game singleton components
		world.AddSingletonComponentType<SunLight>();
		world.AddSingletonComponentType<GameCamera>();
		world.AddSingletonComponentType<CollisionManager>();
		world.AddSingletonComponentType<Game>();
		world.AddSingletonComponentType<SolarEruption>();
		world.AddSingletonComponentType<ClientNetworkManager>();
		// network singleton components
		
		world.AddSingletonComponentType<LinkingContext>();

		world.AddTagType<tag_boundsOutdated>();
		world.AddTagType<tag_sunlight_occlusion>();

		// @hack
		Game& game = world.GetSingletonComponent<Game>();
		game.gameClient = this;
		game.name = _name;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Start()
	{
		game = &world.GetSingletonComponent<Game>();
		netManager = &world.GetSingletonComponent<ClientNetworkManager>();

		netManager->Start( world );

		// Init game
		S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Stop()
	{
		// clears the physics world
		PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
		S_UnregisterAllRigidbodies::Run( world, world.Match( S_UnregisterAllRigidbodies::GetSignature( world ) ) );
		physicsWorld.rigidbodiesHandles.clear();

		// clears the particles mesh
		RenderWorld& renderWorld = world.GetSingletonComponent<RenderWorld>();
		renderWorld.particlesMesh.LoadFromVertices( {} );

		GameCamera::DeleteGameCamera( world );

		netManager->Stop( world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Pause()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameClient::Resume()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Step( const float _delta )
	{
		game->frameIndex++;
		{
			SCOPED_PROFILE( scene_update );			
			netManager->NetworkReceive( world );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), _delta );
			S_MoveFollowTransforms::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );

			// update
			S_RefreshPlayerInput::Run( world, world.Match( S_RefreshPlayerInput::GetSignature( world ) ), _delta );
			netManager->Update( world );
			S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), _delta );
			S_FireWeapons::Run( world, world.Match( S_FireWeapons::GetSignature( world ) ), _delta );
			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), _delta );
			S_GenerateLightMesh::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) ), _delta );
			S_UpdateSolarPannels::Run(world, world.Match( S_UpdateSolarPannels::GetSignature( world ) ), _delta );
			S_RechargeBatteries::Run( world, world.Match( S_RechargeBatteries::GetSignature( world ) ), _delta );
			S_UpdateExpirationTimes::Run( world, world.Match( S_UpdateExpirationTimes::GetSignature( world ) ), _delta );
			S_EruptionDamage::Run( world, world.Match( S_EruptionDamage::GetSignature( world ) ), _delta );
			S_UpdateGameUiValues::Run( world, world.Match( S_UpdateGameUiValues::GetSignature( world ) ), _delta );
			S_UpdateGameUiPosition::Run( world, world.Match( S_UpdateGameUiPosition::GetSignature( world ) ), _delta );

			SolarEruption::Step( world, _delta );

			S_PlayerDeath::Run( world, world.Match( S_PlayerDeath::GetSignature( world ) ), _delta );

			// late update
			S_ParticlesOcclusion::Run( world, world.Match( S_ParticlesOcclusion::GetSignature( world ) ), _delta );
			S_UpdateParticles::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) ), _delta );
			S_EmitParticles::Run( world, world.Match( S_EmitParticles::GetSignature( world ) ), _delta );
			S_GenerateParticles::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromRigidbody::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromModel::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromTransform::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) ), _delta );

			S_UpdateGameCamera::Run( world, world.Match( S_UpdateGameCamera::GetSignature( world ) ), _delta );

			netManager->NetworkSend( world );
		}

		{
			// end frame
			SCOPED_PROFILE( scene_endFrame );
			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}
}