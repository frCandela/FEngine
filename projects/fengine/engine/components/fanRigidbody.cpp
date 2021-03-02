#include "engine/components/fanRigidbody.hpp"
#include "render/fanRenderSerializable.hpp"
#include "engine/singletons/fanPhysicsWorld.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetInfo( EcsComponentInfo& _info )
	{
		_info.destroy = &Rigidbody::Destroy;
		_info.load    = &Rigidbody::Load;
		_info.save    = &Rigidbody::Save;
		_info.netSave= &Rigidbody::NetSave;
		_info.netLoad      = &Rigidbody::NetLoad;
		_info.rollbackLoad = &Rigidbody::RollbackLoad;
		_info.rollbackSave = &Rigidbody::RollbackSave;
	}
	   
	//========================================================================================================
	//========================================================================================================
	void Rigidbody::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		// clear
		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		rb.mRigidbody = new btRigidBody( 1.f, nullptr, nullptr );
		rb.mRigidbody->setUserPointer( &_world );
		rb.mRigidbody->setUserIndex( _world.GetHandle( _entity ) );
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::Destroy( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
		Rigidbody& rb = static_cast<Rigidbody&>( _component );
        fanAssert( rb.mRigidbody != nullptr );

		physicsWorld.mDynamicsWorld->removeRigidBody( rb.mRigidbody );

		delete rb.mRigidbody;
		rb.mRigidbody = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::NetSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const Rigidbody& rb = static_cast<const Rigidbody&>( _component );
		const btVector3 velocity = rb.GetVelocity();
		const btVector3 angularVelocity = rb.GetAngularVelocity();

		_packet << velocity[0] << velocity[2];
		_packet << angularVelocity[1];
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::NetLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		btVector3 velocity( 0.f, 0.f, 0.f );
		btVector3 angularVelocity( 0.f, 0.f, 0.f );
		_packet >> velocity[0] >> velocity[2];
		_packet >> angularVelocity[1] ;
		rb.SetVelocity( velocity );
		rb.SetAngularVelocity( angularVelocity );
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::RollbackSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const Rigidbody&	rb = static_cast<const Rigidbody&>( _component );

		const btVector3&	velocity = rb.GetVelocity();
		const btVector3&	angularVelocity = rb.GetAngularVelocity();
		_packet << velocity[0] << velocity[2];
		_packet << angularVelocity[1];
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::RollbackLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		btVector3		velocity( 0, 0, 0 );
		btVector3		angularVelocity( 0, 0, 0 );

		_packet >> velocity[0] >> velocity[2];
		_packet >> angularVelocity[1];

		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		rb.ClearForces();
		rb.SetVelocity( velocity );
		rb.SetAngularVelocity( angularVelocity );
	}

	//========================================================================================================
	//========================================================================================================
	float  Rigidbody::GetMass() const
	{
		const float invMass = mRigidbody->getInvMass();
		return ( invMass > 0.f ? 1.f / invMass : 0.f );
	}

	//========================================================================================================
	//========================================================================================================
	void  Rigidbody::SetMass( const float _mass )
	{
		btCollisionShape * collisionShape = mRigidbody->getCollisionShape();


		float mass = _mass >= 0.f ? _mass : 0.f;
		btVector3 localInertia( 0.f, 0.f, 0.f );
		if( mass != 0.f && collisionShape )
		{
			collisionShape->calculateLocalInertia( mass, localInertia );
		}
		mRigidbody->setMassProps( mass, localInertia );

		if( mass > 0.f ) 
		{
            mRigidbody->setCollisionFlags( mRigidbody->getCollisionFlags() &
                                           ~btCollisionObject::CF_KINEMATIC_OBJECT );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetStatic()
	{
		SetMass( 0.f );
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetKinematic()
	{
		SetMass( 0.f );
        mRigidbody->setCollisionFlags( mRigidbody->getCollisionFlags() |
                                       btCollisionObject::CF_KINEMATIC_OBJECT );
        mRigidbody->setActivationState( DISABLE_DEACTIVATION );
	}

	//========================================================================================================
	//========================================================================================================
	bool Rigidbody::IsStatic() const
	{
		return !IsKinematic() && mRigidbody->getInvMass() <= 0.f;
	}

	//========================================================================================================
	//========================================================================================================
	bool Rigidbody::IsKinematic() const
	{
		return mRigidbody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::Activate()
	{
		mRigidbody->activate();
	}

	//========================================================================================================
	//========================================================================================================
	bool Rigidbody::IsActive() const
	{
		return mRigidbody->isActive();
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetIgnoreCollisionCheck( const Rigidbody& _rb, const bool state )
	{
		mRigidbody->setIgnoreCollisionCheck( _rb.mRigidbody, state );
	}

	//========================================================================================================
	//========================================================================================================
	bool Rigidbody::IsDeactivationEnabled() const
	{
		return mRigidbody->getActivationState() != DISABLE_DEACTIVATION;
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::EnableDeactivation( const bool _enable )
	{
		mRigidbody->forceActivationState( _enable ? ACTIVE_TAG : DISABLE_DEACTIVATION );
	}

	//========================================================================================================
	//========================================================================================================
    btVector3 Rigidbody::GetPosition() const { return mRigidbody->getWorldTransform().getOrigin(); }
    btQuaternion Rigidbody::GetRotation() const { return mRigidbody->getWorldTransform().getRotation(); }
    const btVector3& Rigidbody::GetVelocity() const { return mRigidbody->getLinearVelocity(); }
    const btVector3& Rigidbody::GetAngularVelocity() const { return mRigidbody->getAngularVelocity(); }
    const btTransform& Rigidbody::GetTransform() const { return mRigidbody->getWorldTransform(); }

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetVelocity( const btVector3& _velocity )
	{
		mRigidbody->setLinearVelocity( _velocity );
	}

    //========================================================================================================
    //========================================================================================================
    void Rigidbody::SetTransform( const btTransform& _transform )
    {
	    mRigidbody->setWorldTransform( _transform );
    }

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetAngularVelocity( const btVector3& _velocity )
	{
		mRigidbody->setAngularVelocity( _velocity );
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetCollisionShape( btCollisionShape* _collisionShape )
	{
		const float mass = GetMass();
		btVector3 localInertia( 0, 0, 0 );
		if( mass != 0.f && _collisionShape != nullptr)
		{
			_collisionShape->calculateLocalInertia( mass, localInertia );
			_collisionShape->setUserPointer( &mRigidbody );
		}
		mRigidbody->setCollisionShape( _collisionShape );
		mRigidbody->setMassProps( mass, localInertia );
	}

	//========================================================================================================
	//========================================================================================================
	void Rigidbody::SetMotionState( btDefaultMotionState* _motionState )
	{		
		mRigidbody->setMotionState( _motionState );
		if( _motionState != nullptr )
		{
			_motionState->m_userPointer = this;
		}
	}

    //========================================================================================================
    //========================================================================================================
    void Rigidbody::Save( const EcsComponent& _component, Json& _json )
    {
        const Rigidbody& rb = static_cast<const Rigidbody&>( _component );
        Serializable::SaveFloat( _json, "mass", rb.GetMass() );
        Serializable::SaveBool( _json, "enable_desactivation", rb.IsDeactivationEnabled() );
        Serializable::SaveBool( _json, "is_kinematic", rb.IsKinematic() );
    }

    //========================================================================================================
    //========================================================================================================
    void Rigidbody::Load( EcsComponent& _component, const Json& _json )
    {
        Rigidbody& rb = static_cast<Rigidbody&>( _component );

        float tmpMass;
        bool tmpEnableDesactivation;
        bool tmpKinematic;

        Serializable::LoadFloat( _json, "mass", tmpMass );
        Serializable::LoadBool( _json, "enable_desactivation", tmpEnableDesactivation );
        Serializable::LoadBool( _json, "is_kinematic", tmpKinematic );

        rb.SetMass( tmpMass );
        rb.EnableDeactivation( tmpEnableDesactivation );
        if( tmpKinematic ) { rb.SetKinematic(); }
    }
}