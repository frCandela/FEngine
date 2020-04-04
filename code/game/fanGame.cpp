#include "game/fanGame.hpp"

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

#include "game/singletonComponents/fanSunLight.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "game/singletonComponents/fanGameReference.hpp"
#include "game/singletonComponents/fanSolarEruption.hpp"

#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"
#include "game/systems/fanUpdateGameCamera.hpp"
#include "game/systems/fanUpdateWeapons.hpp"
#include "game/systems/fanUpdatePlayerInput.hpp"
#include "game/systems/fanUpdateEnergy.hpp"
#include "game/systems/fanUpdateGameUI.hpp"

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
	Game::Game( const std::string _name ) :
		  name( _name )
		, world()
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

		// base singleton components
		world.AddSingletonComponentType<Scene>();
		world.AddSingletonComponentType<RenderWorld>();
		world.AddSingletonComponentType<PhysicsWorld>();
		world.AddSingletonComponentType<ScenePointers>();

		// game singleton components
		world.AddSingletonComponentType<SunLight>();
		world.AddSingletonComponentType<GameCamera>();
		world.AddSingletonComponentType<CollisionManager>();
		world.AddSingletonComponentType<GameReference>();
		world.AddSingletonComponentType<SolarEruption>();

		world.AddTagType<tag_boundsOutdated>();

		// init the game ref
		const_cast< Game* >(world.GetSingletonComponent<GameReference>().game) = this;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Start()
	{
		if( state == State::STOPPED )
		{
			Debug::Highlight() << name << ": play" << Debug::Endl();
			
			// init
			state = State::PLAYING;
			S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );
			GameCamera::CreateGameCamera( world );

			SolarEruption::Start( world );

			// spawn the spaceship
			if( spaceship != nullptr )
			{
				Scene& scene = world.GetSingletonComponent<Scene>();
				SceneNode & spaceshipNode = * spaceship->Instanciate( *scene.root );
				EntityID spaceshipID = world.GetEntityID( spaceshipNode.handle );

				if( world.HasComponent<Transform>( spaceshipID ) 
					&& world.HasComponent<Rigidbody>( spaceshipID ) 
					&& world.HasComponent<MotionState>( spaceshipID )
					&& world.HasComponent<BoxShape>( spaceshipID ))
				{
					// set initial position
					Transform& transform = world.GetComponent<Transform>( spaceshipID );
					transform.SetPosition( btVector3(0,0,4.f) );	

					// add rigidbody to the physics world
					PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
					Rigidbody& rigidbody = world.GetComponent<Rigidbody>( spaceshipID );
					MotionState& motionState = world.GetComponent<MotionState>( spaceshipID );
					BoxShape& boxShape = world.GetComponent<BoxShape>( spaceshipID );
					rigidbody.SetCollisionShape( &boxShape.boxShape );
					rigidbody.SetMotionState( &motionState.motionState );
					physicsWorld.AddRigidbody( rigidbody, spaceshipNode.handle );

					// registers physics callbacks
					CollisionManager& collisionManager = world.GetSingletonComponent<CollisionManager>();
					rigidbody.onContactStarted.Connect( &CollisionManager::OnSpaceShipContact, &collisionManager );
				}
				else
				{
					Debug::Error() 
					<< "Game: spaceship prefab is missing a component" << "\n"
					<< "component needed: Transform, Rigidbody, MotionState, BoxShape" << Debug::Endl();
					return;
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Game::Stop()
	{
		if( state == State::PLAYING || state == State::PAUSED )
		{
			Debug::Highlight() << name << ": stopped" << Debug::Endl();

			state = State::STOPPED;

			// clears the physics world
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_UnregisterAllRigidbodies::Run( world, world.Match( S_UnregisterAllRigidbodies::GetSignature( world ) ) );
			physicsWorld.rigidbodiesHandles.clear();

			// clears the particles mesh
			RenderWorld& renderWorld = world.GetSingletonComponent<RenderWorld>();
			renderWorld.particlesMesh.LoadFromVertices( {} );

			GameCamera::DeleteGameCamera( world );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Pause()
	{
		if( state == State::PLAYING )
		{
			Debug::Highlight() << name << ": paused" << Debug::Endl();
			state = State::PAUSED;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Resume()
	{
		if( state == State::PAUSED )
		{
			Debug::Highlight() << name << ": resumed" << Debug::Endl();
			state = State::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Game::Step( const float _delta )
	{
		{
			SCOPED_PROFILE( scene_update );
			const float delta = ( state == State::PLAYING ? _delta : 0.f );

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), delta );
			physicsWorld.dynamicsWorld->stepSimulation( delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), delta );
			S_MoveFollowTransforms::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );

			// update
			S_RefreshPlayerInput::Run( world, world.Match( S_RefreshPlayerInput::GetSignature( world ) ), delta );
			S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), delta );
			S_FireWeapons::Run( world, world.Match( S_FireWeapons::GetSignature( world ) ), delta );
			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), delta );
			S_GenerateLightMesh::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) ), delta );
			S_UpdateSolarPannels::Run(world, world.Match( S_UpdateSolarPannels::GetSignature( world ) ), delta );
			S_RechargeBatteries::Run( world, world.Match( S_RechargeBatteries::GetSignature( world ) ), delta );
			S_UpdateExpirationTimes::Run( world, world.Match( S_UpdateExpirationTimes::GetSignature( world ) ), delta );
			S_EruptionDamage::Run( world, world.Match( S_EruptionDamage::GetSignature( world ) ), delta );
			S_UpdateGameUiValues::Run( world, world.Match( S_UpdateGameUiValues::GetSignature( world ) ), delta );
			S_UpdateGameUiPosition::Run( world, world.Match( S_UpdateGameUiPosition::GetSignature( world ) ), delta );

			SolarEruption::Step( world, delta );

			S_PlayerDeath::Run( world, world.Match( S_PlayerDeath::GetSignature( world ) ), delta );

			// late update
			S_UpdateParticles::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) ), delta );
			S_EmitParticles::Run( world, world.Match( S_EmitParticles::GetSignature( world ) ), delta );
			S_GenerateParticles::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) ), delta );
			S_UpdateBoundsFromRigidbody::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) ), delta );
			S_UpdateBoundsFromModel::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) ), delta );
			S_UpdateBoundsFromTransform::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) ), delta );

			S_UpdateGameCamera::Run( world, world.Match( S_UpdateGameCamera::GetSignature( world ) ), delta );
		}

		{
			// end frame
			SCOPED_PROFILE( scene_endFrame );
			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}
}