#pragma once

namespace fan {
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================	
	class PhysicsManager {
	public:
		PhysicsManager( const btVector3 _gravity );
		~PhysicsManager();

		void OnGui();

		void StepSimulation( const float _delta );
		void AddRigidbody( btRigidBody * _rigidbody );

	private:

		// Bullet objects
		btDefaultCollisionConfiguration*		m_collisionConfiguration;
		btCollisionDispatcher*					m_dispatcher;
		btBroadphaseInterface*					m_overlappingPairCache;
		btSequentialImpulseConstraintSolver*	m_solver;
		btDiscreteDynamicsWorld*				m_dynamicsWorld;
	};
}