#include "ecs/fanEcsSystem.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanRigidbody.hpp"

namespace fan
{
	//========================================================================================================
	// enumerates over all rigidbody in the world, find the ones that have a collision shape and 
	// optionally a motion state and adds them to the physics world.
	// this can be called post scene load to register all new scene nodes that need it
	// @todo split this in two systems for SphereShape & BoxShape
	//========================================================================================================
	struct SRegisterAllRigidbodies : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<Rigidbody>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
            for( auto rigidbodyIt = _view.begin<Rigidbody>();
                 rigidbodyIt != _view.end<Rigidbody>();
                 ++rigidbodyIt )
			{
				const EcsEntity entity = rigidbodyIt.GetEntity();
				Rigidbody& rb = *rigidbodyIt;

				// find a collision shape
				btCollisionShape* shape = nullptr;
				if( _world.HasComponent<SphereShape>( entity ) )
				{
					shape = _world.GetComponent<SphereShape>( entity ).mSphereShape;
				}
				else if( _world.HasComponent<BoxShape>( entity ) )
				{
					shape = _world.GetComponent<BoxShape>( entity ).mBoxShape;
				}

				// find a motion state
				btDefaultMotionState* motionState = nullptr;
				if( _world.HasComponent<MotionState>( entity ) )
				{
					motionState = _world.GetComponent<MotionState>( entity ).mMotionState;
				}

				// reset the rigidbody				
				rb.SetMotionState( motionState );
				rb.SetCollisionShape( shape );

				physicsWorld.mDynamicsWorld->addRigidBody( rb.mRigidbody );
			}
		}
	};

	//========================================================================================================
	// enumerates over all rigidbody in the world, and removes them from the physics world
	//========================================================================================================
	struct SUnregisterAllRigidbodies : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<Rigidbody>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
            for( auto rigidbodyIt = _view.begin<Rigidbody>();
                 rigidbodyIt != _view.end<Rigidbody>();
                 ++rigidbodyIt )
			{
				Rigidbody& rigidbody = *rigidbodyIt;
				physicsWorld.mDynamicsWorld->removeRigidBody( rigidbody.mRigidbody );
			}
		}
	};
}