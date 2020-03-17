#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"

#include "core/time/fanTime.hpp"
#include "scene/ecs/components/fanRigidbody.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanSphereShape.hpp"
#include "scene/ecs/components/fanBoxShape.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( PhysicsWorld, "physics_world" );

	//================================================================================================================================
	//================================================================================================================================	
	PhysicsWorld::PhysicsWorld()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher( collisionConfiguration );
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, overlappingPairCache, solver, collisionConfiguration );

		gContactStartedCallback = ContactStartedCallback;
		gContactEndedCallback = ContactEndedCallback;

		dynamicsWorld->setGravity( btVector3(0,-10, 0)  /*btVector3::Zero()*/ );

		// Bullet physics is broken when its internal clock is zero, this prevents it from happening when the timestep is exactly equal to the fixed timestep
		dynamicsWorld->stepSimulation( 0.015f, 1, Time::Get().GetPhysicsDelta() );
	}

	//================================================================================================================================
	//================================================================================================================================	
	PhysicsWorld::~PhysicsWorld()
	{
		// remove all collision objects
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			dynamicsWorld->removeCollisionObject( obj );
		}

		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
		//delete dynamicsWorld; @hack
	}

	//================================================================================================================================
	// maybe replace this with a boolean in the component typeinfo ?
	//================================================================================================================================	
	bool PhysicsWorld::IsPhysicsType( EcsWorld& _world, Component& _component )
	{
		return	_world.IsType<Rigidbody>( _component ) ||
				_world.IsType<MotionState>( _component ) ||
				_world.IsType<SphereShape>( _component ) ||
				_world.IsType<BoxShape>( _component );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsWorld::ContactStartedCallback( btPersistentManifold* const& _manifold )
	{
		if( _manifold->getNumContacts() == 1 )
		{
			Rigidbody* rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
			Rigidbody* rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
			rb0->onContactStarted.Emmit( rb1, _manifold );
			rb1->onContactStarted.Emmit( rb0, _manifold );
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsWorld::ContactEndedCallback( btPersistentManifold* const& _manifold )
	{
		if( _manifold->getNumContacts() == 0 )
		{
			Rigidbody* rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
			Rigidbody* rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
			rb0->onContactEnded.Emmit( rb1, _manifold );
			rb1->onContactEnded.Emmit( rb0, _manifold );
		}
	}
}