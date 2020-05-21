#include "scene/systems/fanRegisterPhysics.hpp"

#include "bullet/btBulletDynamicsCommon.h"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_RegisterAllRigidbodies::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_RegisterAllRigidbodies::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities )
	{
		PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
		for( EcsEntity entity : _entities )
		{
			Rigidbody& rb = _world.GetComponent<Rigidbody>( entity );

			// find a collision shape
			btCollisionShape* shape = nullptr;
			if( _world.HasComponent<SphereShape>( entity ) )
			{
				shape = &_world.GetComponent<SphereShape>( entity ).sphereShape;
			}
			else if( _world.HasComponent<BoxShape>( entity ) )
			{
				shape = &_world.GetComponent<BoxShape>( entity ).boxShape;
			}

			// find a motion state
			btDefaultMotionState* motionState = nullptr;
			if( _world.HasComponent<MotionState>( entity ) )
			{
				motionState = &_world.GetComponent<MotionState>( entity ).motionState;
			}

			// reset the rigidbody				
			rb.SetMotionState( motionState );
			rb.SetCollisionShape( shape );

			if( _world.HasComponent<SceneNode>( entity ) )
			{
				SceneNode& sceneNode = _world.GetComponent<SceneNode>( entity );
				physicsWorld.AddRigidbody( rb, sceneNode.handle );
			}
			else
			{
				physicsWorld.dynamicsWorld->addRigidBody( &rb.rigidbody );
			}			
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UnregisterAllRigidbodies::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<Rigidbody>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UnregisterAllRigidbodies::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities )
	{
		PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
		for( EcsEntity entity : _entities )
		{
			Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( entity );
			physicsWorld.RemoveRigidbody( rigidbody );
		}
	}
}