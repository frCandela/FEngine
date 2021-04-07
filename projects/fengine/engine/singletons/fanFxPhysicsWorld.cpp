#include "engine/singletons/fanFxPhysicsWorld.hpp"

#include "core/memory/fanSerializable.hpp"
#include "engine/components/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &FxPhysicsWorld::Save;
        _info.load = &FxPhysicsWorld::Load;
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        FxPhysicsWorld& physicsWorld = static_cast<FxPhysicsWorld&>( _component );
        physicsWorld.mGravity = Vector3( 0, -10, 0 );
        physicsWorld.mDamping = FIXED( 0.995 );
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::ResolveContact( const Contact& _contact, Fixed _duration )
    {
        ResolveVelocity( _contact, _duration );
        ResolveInterpenetration( _contact, _duration );
    }

    //========================================================================================================
    //========================================================================================================
    Fixed FxPhysicsWorld::CalculateSeparatingVelocity( const Contact& _contact )
    {
        Vector3 relativeVelocity = _contact.rb0->mVelocity;
        if( _contact.rb1 ){ relativeVelocity -= _contact.rb1->mVelocity; }
        return Vector3::Dot( relativeVelocity, _contact.normal );
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::ResolveVelocity( const Contact& _contact, Fixed _duration )
    {
        const Fixed separatingVelocity = CalculateSeparatingVelocity( _contact );
        if( separatingVelocity > 0 ){ return; }

        Fixed newSeparatingVelocity = -separatingVelocity * _contact.restitution;

        // check the velocity build up due to acceleration only (causes vibrations on resting contacts)
        Vector3 accCausedVelocity = _contact.rb0->mAcceleration;
        if( _contact.rb1 ){ accCausedVelocity -= _contact.rb1->mAcceleration; }
        const Fixed accCausedSeparatingVelocity = _duration * Vector3::Dot( accCausedVelocity, _contact.normal );
        if( accCausedSeparatingVelocity < 0 )
        {
            newSeparatingVelocity += _contact.restitution * accCausedSeparatingVelocity;
            if( newSeparatingVelocity < 0 ){ newSeparatingVelocity = 0; }
        }

        Fixed totalInverseMass = _contact.rb0->mInverseMass;
        if( _contact.rb1 )
        {
            totalInverseMass += _contact.rb1->mInverseMass;
        }
        if( totalInverseMass <= 0 ){ return; }// infinite mass => immovable

        const Fixed deltaVelocity  = newSeparatingVelocity - separatingVelocity;
        Vector3     impulsePerMass = ( deltaVelocity / totalInverseMass ) * _contact.normal;
        _contact.rb0->mVelocity = _contact.rb0->mVelocity + _contact.rb0->mInverseMass * impulsePerMass;
        if( _contact.rb1 )
        {
            _contact.rb1->mVelocity = _contact.rb1->mVelocity - _contact.rb1->mInverseMass * impulsePerMass;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::ResolveInterpenetration( const Contact& _contact, Fixed _duration )
    {
        (void)_duration;

        if( _contact.penetration <= 0 ){ return; }

        Fixed totalInverseMass = _contact.rb0->mInverseMass;
        if( _contact.rb1 )
        {
            totalInverseMass += _contact.rb1->mInverseMass;
        }
        if( totalInverseMass <= 0 ){ return; }// infinite mass => immovable

        Vector3 movePerInvMass = _contact.normal * _contact.penetration / totalInverseMass;

        _contact.transform0->mPosition = _contact.transform0->mPosition + movePerInvMass * _contact.rb0->mInverseMass;
        if( _contact.rb1 )
        {
            _contact.transform1->mPosition = _contact.transform1->mPosition - movePerInvMass * _contact.rb1->mInverseMass;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::Save( const EcsSingleton& _component, Json& _json )
    {
        const FxPhysicsWorld& physicsWorld = static_cast<const FxPhysicsWorld&>( _component );
        Serializable::SaveVec3( _json, "gravity", physicsWorld.mGravity );
        Serializable::SaveFixed( _json, "damping", physicsWorld.mDamping );
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::Load( EcsSingleton& _component, const Json& _json )
    {
        FxPhysicsWorld& physicsWorld = static_cast<FxPhysicsWorld&>( _component );
        Serializable::LoadVec3( _json, "gravity", physicsWorld.mGravity );
        Serializable::LoadFixed( _json, "damping", physicsWorld.mDamping );
    }
}