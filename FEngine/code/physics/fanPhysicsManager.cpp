#include "fanGlobalIncludes.h"
#include "physics/fanPhysicsManager.h"
#include "scene/components/fanRigidbody.h"

#include "core/time/fanTime.h"
#include "core/time/fanProfiler.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	PhysicsManager::PhysicsManager( const btVector3 _gravity ) {
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher( m_collisionConfiguration );
		m_overlappingPairCache = new btDbvtBroadphase();
		m_solver = new btSequentialImpulseConstraintSolver;
		m_dynamicsWorld = new btDiscreteDynamicsWorld( m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration );

		m_dynamicsWorld->setGravity( _gravity );

		// Bullet physics is broken when its internal clock is zero, this prevents it from happening when the timestep is exactly equal to the fixed timestep
		m_dynamicsWorld->stepSimulation( 0.015f, 1, Time::Get().GetPhysicsDelta() );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::StepSimulation( const float _delta ) {
		SCOPED_PROFILE( physics_steps )
		m_dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::RegisterRigidbody( Rigidbody * _rigidbody ) {
		assert( m_registeredRigidbodies.find( _rigidbody ) == m_registeredRigidbodies.end() );
		m_registeredRigidbodies.insert( _rigidbody );
	}
	
	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::UnRegisterRigidbody( Rigidbody * _rigidbody ) {
		assert( m_registeredRigidbodies.find( _rigidbody ) != m_registeredRigidbodies.end() );
		m_registeredRigidbodies.erase( _rigidbody );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::AddRigidbody( Rigidbody * _rigidbody ) {
		assert( m_rigidbodies.find(_rigidbody) == m_rigidbodies.end() );
		m_rigidbodies.insert(_rigidbody);
		m_dynamicsWorld->addRigidBody( _rigidbody->GetBtBody() );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::RemoveRigidbody( Rigidbody * _rigidbody ) {
		m_rigidbodies.erase( _rigidbody );
		m_dynamicsWorld->removeRigidBody( _rigidbody->GetBtBody() );
	}
	//================================================================================================================================
	//================================================================================================================================	
	PhysicsManager::~PhysicsManager() {
		delete m_dynamicsWorld;
		delete m_solver;
		delete m_overlappingPairCache;
		delete m_dispatcher;
		delete m_collisionConfiguration;		
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::OnGui() {
// 		ImGui::Begin( "Physics" ); {			
// 			ImGui::Text( std::to_string( m_dynamicsWorld->getNumCollisionObjects() ).c_str() );
// 		} ImGui::End();
	}

}