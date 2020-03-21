#include "game/fanGame.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/systems/fanSynchronizeMotionStates.hpp"
//#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "game/systems/fanUpdatePlanets.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Game::Game( const std::string _name, void ( *_initializeTypesEcsWorld )( EcsWorld& ) ) :
		  name( _name )
		, world( _initializeTypesEcsWorld )
		, scene( world.GetSingletonComponent<Scene>() )
	{
		scene.world = &world;
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

			Debug::Highlight() << name << ": stopped" << Debug::Endl();
			state = State::STOPPED;
			//onSceneStop.Emmit( *this );
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
		SCOPED_PROFILE( scene_update );
		{
			//const float delta = m_state == State::PLAYING ? _delta : 0.f;

			//RUN_SYSTEM( ecsPlanetsSystem, Run );

			const Signature signatureSMSFT = S_SynchronizeMotionStateFromTransform::GetSignature( world );
			const Signature signatureSTFMS = S_SynchronizeTransformFromMotionState::GetSignature( world );
			const Signature signatureUpdateParticles = S_UpdateParticles::GetSignature( world );
			const Signature signatureEmitParticles = S_EmitParticles::GetSignature( world );
			const Signature signatureGenParticles = S_GenerateParticles::GetSignature( world );
			const Signature signatureUpdateBoundsFromRigidbody = S_UpdateBoundsFromRigidbody::GetSignature( world );
			const Signature signatureUpdateBoundsFromModel = S_UpdateBoundsFromModel::GetSignature( world );
			const Signature signatureUpdateBoundsFromTransform = S_UpdateBoundsFromTransform::GetSignature( world );
			const Signature signatureMovePlanets = S_MovePlanets::GetSignature( world );
			const Signature signatureGenerateLightMesh = S_GenerateLightMesh::GetSignature( world );

			// physics
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( signatureSMSFT ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( signatureSTFMS ), _delta );

			// particles
			S_UpdateParticles::Run( world, world.Match( signatureUpdateParticles ), _delta );
			S_EmitParticles::Run( world, world.Match( signatureEmitParticles ), _delta );

			S_MovePlanets::Run( world, world.Match( signatureMovePlanets ), _delta );
			S_GenerateLightMesh::Run( world, world.Match( signatureGenerateLightMesh ), _delta );
			S_GenerateParticles::Run( world, world.Match( signatureGenParticles ), _delta );

			//RUN_SYSTEM( ecsSolarEruptionMeshSystem, Run );

			//LateUpdateActors( _delta );
			S_UpdateBoundsFromRigidbody::Run( world, world.Match( signatureUpdateBoundsFromRigidbody ), _delta );
			S_UpdateBoundsFromModel::Run( world, world.Match( signatureUpdateBoundsFromModel ), _delta );
			S_UpdateBoundsFromTransform::Run( world, world.Match( signatureUpdateBoundsFromTransform ), _delta );

			//RUN_SYSTEM( ecsUpdateBullet, Run );
		}

		{
			SCOPED_PROFILE( scene_endFrame );

			// deletes scene nodes
			scene.DeleteNodesImmediate( scene.sceneNodesToDelete );
			scene.sceneNodesToDelete.clear();

			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}
}