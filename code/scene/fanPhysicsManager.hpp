#pragma once

#include "scene/fanScenePrecompiled.hpp"

namespace fan
{
	class EcsManager;
	class Rigidbody;

	//================================================================================================================================
	//================================================================================================================================	
	class PhysicsManager
	{
	public:
		PhysicsManager( const btVector3 _gravity );
		~PhysicsManager();

		void StepSimulation( const float _delta );
		void RegisterRigidbody( Rigidbody* _rigidbody );
		void UnRegisterRigidbody( Rigidbody* _rigidbody );
		void AddRigidbody( Rigidbody* _rigidbody );
		void RemoveRigidbody( Rigidbody* _rigidbody );

	private:
		std::set<Rigidbody*> m_rigidbodies;
		std::set<Rigidbody*> m_registeredRigidbodies;

		// Bullet objects
		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btCollisionDispatcher* m_dispatcher;
		btBroadphaseInterface* m_overlappingPairCache;
		btSequentialImpulseConstraintSolver* m_solver;
		btDiscreteDynamicsWorld* m_dynamicsWorld;

		// 		static bool ContactDestroyedCallback( void* _userPersistentData );
		// 		static bool ContactProcessedCallback( btManifoldPoint& _cp, void* _body0, void* _body1 );
		static void ContactStartedCallback( btPersistentManifold* const& _manifold );
		static void ContactEndedCallback( btPersistentManifold* const& _manifold );

	};
}