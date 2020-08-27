#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_BULLET_PUSH()
#include "bullet/btBulletDynamicsCommon.h"
WARNINGS_POP()

namespace fan
{
	struct Rigidbody;

	//========================================================================================================
	// Contains all Bullet physics components
	// allows registering of rigidbodies and quick rigidbodies access through handles
	// triggers collision callbacks
	//========================================================================================================
	struct PhysicsWorld : public EcsSingleton
	{
		ECS_SINGLETON( PhysicsWorld )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		PhysicsWorld();
		~PhysicsWorld();

		btDefaultCollisionConfiguration*	 mCollisionConfiguration;
		btCollisionDispatcher*				 mDispatcher;
		btDbvtBroadphase*					 mOverlappingPairCache;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld*			 mDynamicsWorld;

		void Reset();
		static void ContactStartedCallback( btPersistentManifold* const& _manifold );
		static void ContactEndedCallback( btPersistentManifold* const& _manifold );
	};
}