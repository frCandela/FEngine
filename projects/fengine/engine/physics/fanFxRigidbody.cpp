#include "engine/physics/fanFxRigidbody.hpp"
#include "core/memory/fanSerializable.hpp"

#include "engine/components/fanFxTransform.hpp"
#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &FxRigidbody::Load;
        _info.save = &FxRigidbody::Save;
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::Init( EcsWorld& _world, EcsEntity, EcsComponent& _component )
    {
        // clear
        FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
        rb.mInverseMass          = 1;
        rb.mInverseInertiaTensor = ( 2 / FIXED( 3 ) / rb.mInverseMass * FIXED( 0.25 ) * Matrix3::sIdentity ).Inverse(); // sphere Inertia = (2/3)*m*r^2
        rb.mVelocity             = Vector3::sZero;
        rb.mRotation             = Vector3::sZero;
        rb.mAcceleration         = _world.GetSingleton<FxPhysicsWorld>().mGravity;
        rb.mForcesAccumulator    = Vector3::sZero;
        rb.mTorqueAccumulator    = Vector3::sZero;
        rb.mTransform            = Matrix4::sIdentity;
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::ApplyForce( const Vector3& _force, const Vector3& _localPoint )
    {
        ApplyCentralForce(_force);
        ApplyTorque( Vector3::Cross( _localPoint, _force));
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::CalculateDerivedData( const FxTransform& _transform )
    {
        mTransform                 = Matrix4( _transform.mRotation, _transform.mPosition );
        mInverseInertiaTensorWorld = Matrix3( _transform.mRotation ) * mInverseInertiaTensor;
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::ClearAccumulators()
    {
        mForcesAccumulator = mTorqueAccumulator = Vector3::sZero;
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::Save( const EcsComponent& _component, Json& _json )
    {
        const FxRigidbody& rb = static_cast<const FxRigidbody&>( _component );
        Serializable::SaveFixed( _json, "inv_mass", rb.mInverseMass );
        Serializable::SaveVec3( _json, "velocity", rb.mVelocity );
        Serializable::SaveVec3( _json, "rotation", rb.mRotation );
        Serializable::SaveVec3( _json, "acceleration", rb.mAcceleration );
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::Load( EcsComponent& _component, const Json& _json )
    {
        FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
        Serializable::LoadFixed( _json, "inv_mass", rb.mInverseMass );
        Serializable::LoadVec3( _json, "velocity", rb.mVelocity );
        Serializable::LoadVec3( _json, "rotation", rb.mRotation );
        Serializable::LoadVec3( _json, "acceleration", rb.mAcceleration );
    }
}

