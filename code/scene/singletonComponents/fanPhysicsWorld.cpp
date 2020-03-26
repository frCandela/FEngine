#include "scene/singletonComponents/fanPhysicsWorld.hpp"

#include "core/time/fanTime.hpp"
#include "core/fanISerializable.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( PhysicsWorld );

	//================================================================================================================================
	//================================================================================================================================
	void PhysicsWorld::SetInfo( SingletonComponentInfo& _info )
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
	void PhysicsWorld::Init( SingletonComponent& _component )
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
	void PhysicsWorld::OnGui( SingletonComponent& _component )
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
		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsWorld::Save( const SingletonComponent& _component, Json& _json )
	{
		const PhysicsWorld& physicsWorld = static_cast<const PhysicsWorld&>( _component );
		Serializable::SaveVec3( _json, "gravity", physicsWorld.dynamicsWorld->getGravity() );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsWorld::Load( SingletonComponent& _component, const Json& _json )
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

		gContactStartedCallback = ContactStartedCallback;
		gContactEndedCallback = ContactEndedCallback;

		dynamicsWorld->setGravity( btVector3::Zero() );

		// Bullet physics is broken when its internal clock is zero, this prevents it from happening when the timestep is exactly equal to the fixed timestep
		dynamicsWorld->stepSimulation( 0.015f, 1, Time::Get().GetPhysicsDelta() );
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

		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
		delete dynamicsWorld; //@hack bullet physics alignement is broken, check the lib compilation parameters...
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