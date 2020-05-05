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

			if( _world.HasComponent<SceneNode>( entityID ) )
			{
				SceneNode& sceneNode = _world.GetComponent<SceneNode>( entityID );
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
			physicsWorld.RemoveRigidbody( rigidbody );
		}
	}
}