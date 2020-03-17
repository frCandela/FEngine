#include "scene/ecs/systems/fanRegisterPhysics.hpp"

#include "scene/ecs/components/fanSphereShape.hpp"
#include "scene/ecs/components/fanBoxShape.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanRigidbody.hpp"
#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	Signature S_RegisterAllRigidbodies::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_RegisterAllRigidbodies::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
		for( EntityID entityID : _entities )
		{
			Rigidbody& rb = _world.GetComponent<Rigidbody>( entityID );

			// find a collision shape
			btCollisionShape* shape = nullptr;
			if( _world.HasComponent<SphereShape>( entityID ) )
			{
				shape = &_world.GetComponent<SphereShape>( entityID ).sphereShape;
			}
			else if( _world.HasComponent<BoxShape>( entityID ) )
			{
				shape = &_world.GetComponent<BoxShape>( entityID ).boxShape;
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
		return _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UnregisterAllRigidbodies::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
		for( EntityID entityID : _entities )
		{
			Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( entityID );
			physicsWorld.dynamicsWorld->removeRigidBody( &rigidbody.rigidbody );
		}
	}
}