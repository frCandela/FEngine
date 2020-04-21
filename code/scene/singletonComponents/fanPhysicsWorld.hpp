#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct Rigidbody;
	struct MotionState;

	//================================================================================================================================
	// Contains all Bullet physics components
	// allows registering of rigidbodies and quick rigidbodies access through handles
	// triggers collision callbacks
	//================================================================================================================================
	struct PhysicsWorld : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		PhysicsWorld();
		~PhysicsWorld();

		btDefaultCollisionConfiguration*	 collisionConfiguration;
		btCollisionDispatcher*				 dispatcher;
		btDbvtBroadphase*					 overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld*			 dynamicsWorld;
		std::unordered_map< Rigidbody*, EntityHandle > rigidbodiesHandles;

		void AddRigidbody( Rigidbody& _rigidbody, EntityHandle _entityID );
		void RemoveRigidbody( Rigidbody& _rigidbody );

		static void ContactStartedCallback( btPersistentManifold* const& _manifold );
		static void ContactEndedCallback( btPersistentManifold* const& _manifold );
	};
}