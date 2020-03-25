#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

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
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

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