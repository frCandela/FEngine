#include "scene/singletonComponents/fanPhysicsWorld.hpp"

#include "core/time/fanTime.hpp"
#include "core/fanSerializable.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void PhysicsWorld::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::RIGIDBODY16;
		_info.init = &PhysicsWorld::Init;
		_info.onGui = &PhysicsWorld::OnGui;
		_info.save = &PhysicsWorld::Save;
		_info.load = &PhysicsWorld::Load;
		_info.name = "physics world";
	}

	//================================================================================================================================
	//================================================================================================================================
	void PhysicsWorld::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );
		
		// remove all collision objects
		for( int i = physicsWorld.dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i-- )
		{
			btCollisionObject* obj = physicsWorld.dynamicsWorld->getCollisionObjectArray()[i];
			physicsWorld.dynamicsWorld->removeCollisionObject( obj );
		}
		physicsWorld.dynamicsWorld->setGravity( btVector3::Zero() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PhysicsWorld::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			btVector3 gravity = physicsWorld.dynamicsWorld->getGravity();
			if( ImGui::DragFloat3( "gravity", &gravity[0], 0.1f, -20.f, 20.f ) )
			{
				physicsWorld.dynamicsWorld->setGravity( gravity );
			}
			ImGui::Text( "num rigidbodies : %d", physicsWorld.dynamicsWorld->getNumCollisionObjects() );
			ImGui::Text( "num handles:      %d", physicsWorld.rigidbodiesHandles.size() );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsWorld::Save( const EcsSingleton& _component, Json& _json )
	{
		const PhysicsWorld& physicsWorld = static_cast<const PhysicsWorld&>( _component );
		Serializable::SaveVec3( _json, "gravity", physicsWorld.dynamicsWorld->getGravity() );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsWorld::Load( EcsSingleton& _component, const Json& _json )
	{
		btVector3 gravity;
		Serializable::LoadVec3(	 _json, "gravity", gravity );
		PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );
		physicsWorld.dynamicsWorld->setGravity( gravity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	PhysicsWorld::PhysicsWorld()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher( collisionConfiguration );
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, overlappingPairCache, solver, collisionConfiguration );

		// deterministic configuration ( not sure if it helps -> please test as you need it )
// 		btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
// 		info.m_solverMode = info.m_solverMode & ~( SOLVER_USE_WARMSTARTING );	// not tested
// 		dynamicsWorld->getSimulationIslandManager()->setSplitIslands( false );  // not tested

		gContactStartedCallback = ContactStartedCallback;
		gContactEndedCallback = ContactEndedCallback;
		dynamicsWorld->setGravity( btVector3::Zero() );

		// Bullet physics is broken when its internal clock is zero, this prevents it from happening when the timestep is exactly equal to the fixed timestep
		dynamicsWorld->stepSimulation( 0.015f, 1, Time::Get().GetPhysicsDelta() );
	}

	//================================================================================================================================
	// this is untested copy pasted code, for inspiration before implementing the real feature
	//================================================================================================================================	
	void PhysicsWorld::Reset()
	{
		///create a copy of the array, not a reference!
		const btCollisionObjectArray& collisionObjects = dynamicsWorld->getCollisionObjectArray();
		for( int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++ )
		{
			btCollisionObject* colObj = collisionObjects[i];
			btRigidBody* body = btRigidBody::upcast( colObj );
			if( body )
			{
// 				if( body->getMotionState() )
// 				{
// 					btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
// 					myMotionState->m_graphicsWorldTrans = myMotionState->m_startWorldTrans;
// 					body->setCenterOfMassTransform( myMotionState->m_graphicsWorldTrans );
// 					colObj->setInterpolationWorldTransform( myMotionState->m_startWorldTrans );
// 					colObj->forceActivationState( ACTIVE_TAG );
// 					colObj->activate();
// 					colObj->setActivationState( DISABLE_DEACTIVATION );
// 					colObj->setDeactivationTime( btScalar( 2e7 ) );
// 				}
				//removed cached contact points (this is not necessary if all objects have been removed from the dynamics world)
				dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs( colObj->getBroadphaseHandle(), dynamicsWorld->getDispatcher() );
// 
// 				btRigidBody* body = btRigidBody::upcast( colObj );
// 				if( body && !body->isStaticObject() )
// 				{
// 					btRigidBody::upcast( colObj )->setLinearVelocity( btVector3( 0, 0, 0 ) );
// 					btRigidBody::upcast( colObj )->setAngularVelocity( btVector3( 0, 0, 0 ) );
// 					btRigidBody::upcast( colObj )->clearForces();
// 				}
			}
		}

		// reset some internal cached data in the broad phase
		dynamicsWorld->getBroadphase()->resetPool( dynamicsWorld->getDispatcher() );
		dynamicsWorld->getConstraintSolver()->reset();
	}

	//================================================================================================================================
	//================================================================================================================================	
	PhysicsWorld::~PhysicsWorld()
	{
		for( int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i-- )
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			dynamicsWorld->removeCollisionObject( obj );
		}
		rigidbodiesHandles.clear();

		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
		delete dynamicsWorld;
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

	//================================================================================================================================
	// adds a rigidbody to the dynamics world and saves its handle ( useful for later use in collision callbacks )
	//================================================================================================================================	
	void PhysicsWorld::AddRigidbody( Rigidbody& _rigidbody, EcsHandle _handle )
	{
		rigidbodiesHandles[&_rigidbody] = _handle;
		dynamicsWorld->addRigidBody( &_rigidbody.rigidbody );
	}

	//================================================================================================================================
	// removes a rigidbody from the dynamics world and erases its handle
	//================================================================================================================================	
	void PhysicsWorld::RemoveRigidbody( Rigidbody& _rigidbody )
	{
		auto it = rigidbodiesHandles.find( &_rigidbody );
		if( it != rigidbodiesHandles.end() )
		{
			rigidbodiesHandles.erase( it );
		}
		dynamicsWorld->removeRigidBody( &_rigidbody.rigidbody );
	}
}