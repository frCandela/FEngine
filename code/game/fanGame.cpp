#include "game/fanGame.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"

#include "game/singletonComponents/fanSunLight.hpp"

#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"

#include "game/components/fanPlanet.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanPlayerInput.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Game::Game( const std::string _name ) :
		  name( _name )
		, world()
	{
		world.AddSingletonComponentType<SunLight>();

		world.AddComponentType<Planet>();
		world.AddComponentType<SpaceShip>();
		world.AddComponentType<PlayerInput>();
	}

	//================================================================================================================================
	//================================================================================================================================
	Game::~Game()
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void Game::Play()
	{
		// unregister / register rigidbodies		
		S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );

		if( state == State::STOPPED )
		{
			Debug::Highlight() << name << ": play" << Debug::Endl();
			state = State::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Game::Stop()
	{
		if( state == State::PLAYING || state == State::PAUSED )
		{
			S_UnregisterAllRigidbodies::Run( world, world.Match( S_UnregisterAllRigidbodies::GetSignature( world ) ) );
			Debug::Highlight() << name << ": stopped" << Debug::Endl();
			state = State::STOPPED;
			onStop.Emmit( *this );
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

			// physics
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), delta );
			physicsWorld.dynamicsWorld->stepSimulation( delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), delta );

			// update
			S_UpdateSpaceships::Run( world, world.Match( S_UpdateSpaceships::GetSignature( world ) ), delta );
			S_UpdateParticles::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) ), delta );
			S_EmitParticles::Run( world, world.Match( S_EmitParticles::GetSignature( world ) ), delta );

			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), delta );
			S_GenerateLightMesh::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) ), delta );
			S_GenerateParticles::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) ), delta );
			//RUN_SYSTEM( ecsSolarEruptionMeshSystem, Run );
			

			// late update
			S_UpdateBoundsFromRigidbody::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) ), delta );
			//RUN_SYSTEM( ecsUpdateBullet, Run );			
			S_UpdateBoundsFromModel::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) ), delta );
			S_UpdateBoundsFromTransform::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) ), delta );
		}

		{
			// end frame
			SCOPED_PROFILE( scene_endFrame );
			Scene& scene = world.GetSingletonComponent<Scene>();
			scene.DeleteNodesImmediate( scene.sceneNodesToDelete );
			scene.sceneNodesToDelete.clear();
			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}
}