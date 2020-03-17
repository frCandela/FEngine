#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct Rigidbody;
	struct MotionState;

	//================================================================================================================================
	//================================================================================================================================
	struct PhysicsWorld : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		PhysicsWorld();
		~PhysicsWorld();

		btDefaultCollisionConfiguration*	 collisionConfiguration;
		btCollisionDispatcher*				 dispatcher;
		btDbvtBroadphase*					 overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld*			 dynamicsWorld;

		static void ContactStartedCallback( btPersistentManifold* const& _manifold );
		static void ContactEndedCallback( btPersistentManifold* const& _manifold );

		static bool IsPhysicsType( EcsWorld& _world, Component& _component );
	};
}