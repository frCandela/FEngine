#include "fanGlobalIncludes.h"
#include "physics/fanPhysicsManager.h"
#include "scene/components/fanRigidbody.h"
#include "core/fanTime.h"

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
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::StepSimulation( const float _delta ) {
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
	void PhysicsManager::Clear() {
		Debug::Warning( "clear physics" );
		for (int  bodyIndex = m_dynamicsWorld->getNumCollisionObjects() - 1; bodyIndex >= 0; bodyIndex-- ) {
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[bodyIndex];
			m_dynamicsWorld->removeCollisionObject( obj );
		}
	}
	//================================================================================================================================
	// Clears the world and add the refreshed rigidbodies
	// Happens when too much rigidbodies are created in the ecs and their container is resized
	//================================================================================================================================	
	void PhysicsManager::Refresh() {
		Debug::Warning("refresh physics");
		std::set<Rigidbody*> copySet( m_registeredRigidbodies );
		for ( Rigidbody * rb : copySet ){
			rb->Refresh();
			m_dynamicsWorld->addRigidBody( rb->GetBtBody() );
		}
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
// 			if ( ImGui::Button( "clear" ) ) {
// 				Clear();
// 			}
// 
// 		} ImGui::End();
	}

}