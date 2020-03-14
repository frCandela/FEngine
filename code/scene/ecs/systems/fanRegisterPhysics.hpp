#include "scene/ecs/fanSystem.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

#include "scene/ecs/components/fanSphereShape2.hpp"
#include "scene/ecs/components/fanBoxShape2.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanRigidbody2.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// enumerates over all rigidbody in the world, find the ones that have a collision shape and 
	// optionally a motion state and adds them to the physics world.
	// this can be called post scene load to register all new scene nodes that need it
	//==============================================================================================================================================================
	struct S_RegisterAllRigidbodies : System
	{
		static Signature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<Rigidbody2>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
			for( EntityID entityID : _entities )
			{
				Rigidbody2& rb = _world.GetComponent<Rigidbody2>( entityID );

				// find a collision shape
				btCollisionShape* shape = nullptr;
				if( _world.HasComponent<SphereShape2>( entityID ) )
				{
					shape = &_world.GetComponent<SphereShape2>( entityID ).sphereShape;
				}
				else if( _world.HasComponent<BoxShape2>( entityID ) )
				{
					shape = &_world.GetComponent<BoxShape2>( entityID ).boxShape;
				}

				// find a motion state
				btDefaultMotionState* motionState = nullptr;
				if( _world.HasComponent<MotionState>( entityID ) )
				{
					motionState = &_world.GetComponent<MotionState>( entityID ).motionState;
				}

				// reset the rigidbody				
				rb.SetMotionState( motionState );
				rb.SetCollisionShape( shape );
				physicsWorld.dynamicsWorld->addRigidBody( &rb.rigidbody );				
			}
		}
	};

	//==============================================================================================================================================================
	// enumerates over all rigidbody in the world, and removes them from the physics world
	//==============================================================================================================================================================
	struct S_UnregisterAllRigidbodies : System
	{
		static Signature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<Rigidbody2>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
			for( EntityID entityID : _entities )
			{
				Rigidbody2& rigidbody = _world.GetComponent<Rigidbody2>( entityID );
				physicsWorld.dynamicsWorld->removeRigidBody( &rigidbody.rigidbody );				
			}
		}
	};
}