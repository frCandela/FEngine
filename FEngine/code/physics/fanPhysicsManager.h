#pragma once

namespace fan {
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================	
	class PhysicsManager {
		friend class Engine;
	public:
		PhysicsManager( const btVector3 _gravity );
		~PhysicsManager();

		void OnGui();

	private:
		// External references
		EcsManager * m_ecsManager = nullptr;

		// Bullet objects
		btDefaultCollisionConfiguration*		m_collisionConfiguration;
		btCollisionDispatcher*					m_dispatcher;
		btBroadphaseInterface*					m_overlappingPairCache;
		btSequentialImpulseConstraintSolver*	m_solver;
		btDiscreteDynamicsWorld*				m_dynamicsWorld;
	};
}