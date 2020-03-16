#include "scene/ecs/systems/fanRegisterPhysics.hpp"

#include "scene/ecs/components/fanSphereShape2.hpp"
#include "scene/ecs/components/fanBoxShape2.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanRigidbody2.hpp"
#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	Signature S_RegisterAllRigidbodies::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<Rigidbody2>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_RegisterAllRigidbodies::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
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


	//================================================================================================================================
	//================================================================================================================================
	Signature S_UnregisterAllRigidbodies::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<Rigidbody2>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UnregisterAllRigidbodies::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
		for( EntityID entityID : _entities )
		{
			Rigidbody2& rigidbody = _world.GetComponent<Rigidbody2>( entityID );
			physicsWorld.dynamicsWorld->removeRigidBody( &rigidbody.rigidbody );
		}
	}
}