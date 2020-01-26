#include "fanGlobalIncludes.h"
#include "physics/fanPhysicsManager.h"

#include "core/time/fanTime.h"
#include "core/time/fanProfiler.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanRigidbody.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	PhysicsManager::PhysicsManager( const btVector3 _gravity ) {
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher( m_collisionConfiguration );
		m_overlappingPairCache = new btDbvtBroadphase();
		m_solver = new btSequentialImpulseConstraintSolver;
		m_dynamicsWorld = new btDiscreteDynamicsWorld( m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration );

// 		gContactDestroyedCallback	= ContactDestroyedCallback;
// 		gContactProcessedCallback = ContactProcessedCallback;
		gContactStartedCallback = ContactStartedCallback;
		gContactEndedCallback = ContactEndedCallback;

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
// 
// 	bool PhysicsManager::ContactDestroyedCallback( void* _userPersistentData ){	return true;}
// 	bool PhysicsManager::ContactProcessedCallback( btManifoldPoint& _cp, void* _body0, void* _body1 ) {	return true;}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::ContactStartedCallback	( btPersistentManifold* const& _manifold )
	{
		if ( _manifold->getNumContacts() == 1 )	{
			Rigidbody * rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
			Rigidbody * rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
			rb0->onContactStarted.Emmit( rb1, _manifold );
			rb1->onContactStarted.Emmit( rb0, _manifold );
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::ContactEndedCallback	( btPersistentManifold* const& _manifold )
	{
		if ( _manifold->getNumContacts() == 0 )
		{
			Rigidbody * rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
			Rigidbody * rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
			rb0->onContactEnded.Emmit( rb1, _manifold );
			rb1->onContactEnded.Emmit( rb0, _manifold );
		}
	}
}