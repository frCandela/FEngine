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
#include "game/fanGameTags.hpp"
#include "network/fanPacket.hpp"

#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/systems/fanClientUpdates.hpp"
#include "network/systems/fanClientSendReceive.hpp"
#include "network/systems/fanTimeout.hpp"

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
		world.AddComponentType<ClientGameData>();

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
	void GameClient::RollbackResimulate( EcsWorld& _world )
	{
		const EntityID persistentID = _world.GetEntityID( netManager->playerPersistent->handle );
		ClientGameData& gameData = _world.GetComponent<ClientGameData>( persistentID );
		if( !gameData.spaceshipSynced && ! gameData.previousInputsSinceLastGameState.empty() )
		{
			const PacketInput::InputData mostRecent = gameData.previousInputsSinceLastGameState.front();
			const PacketInput::InputData oldest = gameData.previousInputsSinceLastGameState.back();

			if( mostRecent.frameIndex == game->frameIndex && oldest.frameIndex == gameData.lastServerState.frameIndex )
			{
				const float delta = game->logicDelta;

				// Rollback at the frame we took the snapshot of the player game state
				game->frameIndex = oldest.frameIndex;
				Debug::Highlight() << "rollback to frame " << oldest.frameIndex << Debug::Endl();
				const EntityID spaceshipID = _world.GetEntityID( gameData.spaceshipHandle );
				PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();

				Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( spaceshipID );
				physicsWorld.dynamicsWorld->removeRigidBody( &rigidbody.rigidbody );
				physicsWorld.dynamicsWorld->addRigidBody( &rigidbody.rigidbody );
				rigidbody.rigidbody.clearForces();

				Transform& transform = _world.GetComponent<Transform>( spaceshipID );
				rigidbody.rigidbody.clearForces();
				rigidbody.SetVelocity(			gameData.lastServerState.velocity			);
				rigidbody.SetAngularVelocity(	gameData.lastServerState.angularVelocity	);
				transform.SetPosition(			gameData.lastServerState.position			);
				transform.SetRotationEuler(		gameData.lastServerState.orientation		);
				S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), delta );
				
				PlayerInput& input = _world.GetComponent<PlayerInput>( spaceshipID );
				gameData.previousStates = std::queue< PacketPlayerGameState >(); // clear previous states
				gameData.previousStates.push( gameData.lastServerState );

				const PacketInput::InputData& inputData = *( gameData.previousInputsSinceLastGameState.rbegin() + 0 );
				assert( inputData.frameIndex == game->frameIndex );
				input.orientation = btVector3( inputData.orientation.x, 0.f, inputData.orientation.y );
				input.left = inputData.left ? 1.f : ( inputData.right ? -1.f : 0.f );
				input.forward = inputData.forward ? 1.f : ( inputData.backward ? -1.f : 0.f );
				input.boost = inputData.boost;
				input.fire = inputData.fire;

				S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), delta );

				for (int i = 1; i < gameData.previousInputsSinceLastGameState.size(); i++)
				{
					game->frameIndex++;

					S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), delta );
					physicsWorld.dynamicsWorld->stepSimulation( game->logicDelta, 10, Time::Get().GetPhysicsDelta() );
					S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), delta );

					const PacketInput::InputData& inputData = *( gameData.previousInputsSinceLastGameState.rbegin() + i );
					assert( inputData.frameIndex == game->frameIndex );
					input.orientation = btVector3( inputData.orientation.x, 0.f, inputData.orientation.y );
					input.left = inputData.left ? 1.f : ( inputData.right ? -1.f : 0.f );
					input.forward = inputData.forward ? 1.f : ( inputData.backward ? -1.f : 0.f );
					input.boost = inputData.boost;
					input.fire = inputData.fire;

					S_ClientSaveState::Run( world, world.Match( S_ClientSaveState::GetSignature( world ) ), delta );

					S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), delta );
					S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), delta );
 				}	
				gameData.spaceshipSynced = true;
				assert( game->frameIndex == mostRecent.frameIndex );
 			}		
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameClient::Step( const float _delta )
	{		
		{
			SCOPED_PROFILE( scene_update );			
			S_ClientReceive::Run( world, world.Match( S_ClientReceive::GetSignature( world ) ), _delta );

			RollbackResimulate( world );

			if( _delta > 0.f )
			{
				game->frameIndex++;
			}

			S_ProcessTimedOutPackets	::Run( world, world.Match( S_ProcessTimedOutPackets::GetSignature( world ) ) );
			S_ClientDetectServerTimeout	::Run( world, world.Match( S_ClientDetectServerTimeout::GetSignature( world ) )	, _delta );
			S_ClientRunReplication		::Run( world, world.Match( S_ClientRunReplication::GetSignature( world ) )		, _delta );
			S_ClientSpawnSpaceship		::Run( world, world.Match( S_ClientSpawnSpaceship::GetSignature( world ) )		, _delta );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform	::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState	::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), _delta );
			S_MoveFollowTransforms					::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI				::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );			

			// update
			S_RefreshPlayerInput	::Run( world, world.Match( S_RefreshPlayerInput::GetSignature( world ) )	, _delta );
			S_ClientSaveInput::Run( world, world.Match( S_ClientSaveInput::GetSignature( world ) ), _delta );
			S_ClientSaveState::Run( world, world.Match( S_ClientSaveState::GetSignature( world ) ), _delta );

			S_MoveSpaceships		::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) )		, _delta );
			S_FireWeapons			::Run( world, world.Match( S_FireWeapons::GetSignature( world ) )			, _delta );
			S_MovePlanets			::Run( world, world.Match( S_MovePlanets::GetSignature( world ) )			, _delta );
			S_GenerateLightMesh		::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) )		, _delta );
			S_UpdateSolarPannels	::Run(world, world.Match( S_UpdateSolarPannels::GetSignature( world ) )		, _delta );
			S_RechargeBatteries		::Run( world, world.Match( S_RechargeBatteries::GetSignature( world ) )		, _delta );
			S_UpdateExpirationTimes	::Run( world, world.Match( S_UpdateExpirationTimes::GetSignature( world ) )	, _delta );
			S_EruptionDamage		::Run( world, world.Match( S_EruptionDamage::GetSignature( world ) )		, _delta );
			S_UpdateGameUiValues	::Run( world, world.Match( S_UpdateGameUiValues::GetSignature( world ) )	, _delta );
			S_UpdateGameUiPosition	::Run( world, world.Match( S_UpdateGameUiPosition::GetSignature( world ) )	, _delta );
			SolarEruption			::Step( world, _delta );
			S_PlayerDeath			::Run( world, world.Match( S_PlayerDeath::GetSignature( world ) ), _delta );

			// late update
			S_ParticlesOcclusion		::Run( world, world.Match( S_ParticlesOcclusion::GetSignature( world ) )		, _delta );
			S_UpdateParticles			::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) )			, _delta );
			S_EmitParticles				::Run( world, world.Match( S_EmitParticles::GetSignature( world ) )				, _delta );
			S_GenerateParticles			::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) )			, _delta );
			S_UpdateBoundsFromRigidbody	::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) )	, _delta );
			S_UpdateBoundsFromModel		::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) )		, _delta );
			S_UpdateBoundsFromTransform	::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) )	, _delta );
			S_UpdateGameCamera			::Run( world, world.Match( S_UpdateGameCamera::GetSignature( world ) )			, _delta );
			
			S_ClientSend		::Run( world, world.Match( S_ClientSend::GetSignature( world ) )  , _delta );
		}

		{
			// end frame
			SCOPED_PROFILE( scene_endFrame );
			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}
}