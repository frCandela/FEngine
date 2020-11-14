#include "scene/singletons/fanPhysicsWorld.hpp"

#include "network/singletons/fanTime.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::SetInfo( EcsSingletonInfo& _info )
	{
		_info.save   = &PhysicsWorld::Save;
		_info.load   = &PhysicsWorld::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );
		
		// remove all collision objects
		for( int i = physicsWorld.mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i-- )
		{
			btCollisionObject* obj = physicsWorld.mDynamicsWorld->getCollisionObjectArray()[i];
			physicsWorld.mDynamicsWorld->removeCollisionObject( obj );
		}
		physicsWorld.mDynamicsWorld->setGravity( btVector3::Zero() );
	}

	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::Save( const EcsSingleton& _component, Json& _json )
	{
		const PhysicsWorld& physicsWorld = static_cast<const PhysicsWorld&>( _component );
		Serializable::SaveVec3( _json, "gravity", physicsWorld.mDynamicsWorld->getGravity() );
	}

	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::Load( EcsSingleton& _component, const Json& _json )
	{
		btVector3 gravity;
		Serializable::LoadVec3(	 _json, "gravity", gravity );
		PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );
		physicsWorld.mDynamicsWorld->setGravity( gravity );
	}

	//========================================================================================================
	//========================================================================================================
	PhysicsWorld::PhysicsWorld()
	{
        mCollisionConfiguration = new btDefaultCollisionConfiguration();
        mDispatcher             = new btCollisionDispatcher( mCollisionConfiguration );
        mOverlappingPairCache   = new btDbvtBroadphase();
        mSolver                 = new btSequentialImpulseConstraintSolver();
        mDynamicsWorld = new btDiscreteDynamicsWorld( mDispatcher,
                                                      mOverlappingPairCache,
                                                      mSolver,
                                                      mCollisionConfiguration );

		// deterministic configuration ( not sure if it helps -> please test as you need it )
// 		btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
// 		info.m_solverMode = info.m_solverMode & ~( SOLVER_USE_WARMSTARTING );	// not tested
// 		dynamicsWorld->getSimulationIslandManager()->setSplitIslands( false );  // not tested

		gContactStartedCallback = ContactStartedCallback;
		gContactEndedCallback = ContactEndedCallback;
		mDynamicsWorld->setGravity( btVector3::Zero() );

		// Bullet physics is broken when its internal clock is zero,
		// this prevents it from happening when the timestep is exactly equal to the fixed timestep
		mDynamicsWorld->stepSimulation( 0.015f, 1, Time::sPhysicsDelta );
	}

	//========================================================================================================
	// this is untested copy pasted code, for inspiration before implementing the real feature
	//========================================================================================================
	void PhysicsWorld::Reset()
	{
		///create a copy of the array, not a reference!
		const btCollisionObjectArray& collisionObjects = mDynamicsWorld->getCollisionObjectArray();
		for( int i = 0; i < mDynamicsWorld->getNumCollisionObjects(); i++ )
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
				// removed cached contact points
				// (this is not necessary if all objects have been removed from the dynamics world)
                btOverlappingPairCache* pairCache = mDynamicsWorld->getBroadphase()->getOverlappingPairCache();
                pairCache->cleanProxyFromPairs( colObj->getBroadphaseHandle(), mDynamicsWorld->getDispatcher());
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
		mDynamicsWorld->getBroadphase()->resetPool( mDynamicsWorld->getDispatcher() );
		mDynamicsWorld->getConstraintSolver()->reset();
	}

	//========================================================================================================
	//========================================================================================================
	PhysicsWorld::~PhysicsWorld()
	{
		for( int i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i-- )
		{
			btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
			mDynamicsWorld->removeCollisionObject( obj );
		}

		delete mSolver;
		delete mOverlappingPairCache;
		delete mDispatcher;
		delete mCollisionConfiguration;
		delete mDynamicsWorld;
	}

	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::ContactStartedCallback( btPersistentManifold* const& _manifold )
	{
		if( _manifold->getNumContacts() == 1 )
		{
			EcsWorld& world = * static_cast<EcsWorld*> ( _manifold->getBody0()->getUserPointer() );

			const EcsHandle rb0Handle = static_cast<EcsHandle> ( _manifold->getBody0()->getUserIndex() );
			const EcsHandle rb1Handle = static_cast<EcsHandle> ( _manifold->getBody1()->getUserIndex() );
			Rigidbody& rb0 = world.GetComponent< Rigidbody >( world.GetEntity( rb0Handle ) );
			Rigidbody& rb1 = world.GetComponent< Rigidbody >( world.GetEntity( rb1Handle ) );
			rb0.mOnContactStarted.Emmit( rb0, rb1, _manifold );
			rb1.mOnContactStarted.Emmit( rb1, rb0, _manifold );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::ContactEndedCallback( btPersistentManifold* const& _manifold )
	{
		if( _manifold->getNumContacts() == 0 )
		{
			EcsWorld& world = *static_cast<EcsWorld*> ( _manifold->getBody0()->getUserPointer() );

			const EcsHandle rb0Handle = static_cast<EcsHandle> ( _manifold->getBody0()->getUserIndex() );
			const EcsHandle rb1Handle = static_cast<EcsHandle> ( _manifold->getBody1()->getUserIndex() );
			Rigidbody& rb0 = world.GetComponent< Rigidbody >( world.GetEntity( rb0Handle ) );
			Rigidbody& rb1 = world.GetComponent< Rigidbody >( world.GetEntity( rb1Handle ) );
			rb0.mOnContactEnded.Emmit( rb0, rb1, _manifold );
			rb1.mOnContactEnded.Emmit( rb1, rb0, _manifold );
		}
	}
}