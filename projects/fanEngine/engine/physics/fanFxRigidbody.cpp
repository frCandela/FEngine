#include <core/fanDebug.hpp>
#include "engine/physics/fanFxRigidbody.hpp"
#include "fanFxTransform.hpp"
#include "engine/physics/fanFxPhysicsWorld.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &FxRigidbody::Load;
        _info.save = &FxRigidbody::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::Init( EcsWorld& _world, EcsEntity, EcsComponent& _component )
    {
        // clear
        const FxPhysicsWorld& physicsWorld = _world.GetSingleton<FxPhysicsWorld>();
        FxRigidbody         & rb           = static_cast<FxRigidbody&>( _component );
        rb.mInverseMass               = 1;
        rb.mInverseInertiaTensorLocal = Matrix3::sZero;
        rb.mVelocity                  = Vector3::sZero;
        rb.mRotation                  = Vector3::sZero;
        rb.mAcceleration              = physicsWorld.mGravity;
        rb.mForcesAccumulator         = Vector3::sZero;
        rb.mTorqueAccumulator         = Vector3::sZero;
        rb.mTransform                 = nullptr;
        rb.mCanSleep                  = true;
        rb.mIsSleeping                = false;
        rb.mMotion                    = 4 * sSleepEpsilon;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::ApplyForce( const Vector3& _force, const Vector3& _localPoint )
    {
        ApplyCentralForce( _force );
        ApplyTorque( Vector3::Cross( _localPoint, _force ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TransformInertiaTensor( const Matrix3& _intITLocal, const Matrix4& _transform, Matrix3& _outInvITWorld )
    {
        Fixed t4  = _transform.e11 * _intITLocal.e11 + _transform.e12 * _intITLocal.e21 + _transform.e13 * _intITLocal.e31;
        Fixed t9  = _transform.e11 * _intITLocal.e12 + _transform.e12 * _intITLocal.e22 + _transform.e13 * _intITLocal.e32;
        Fixed t14 = _transform.e11 * _intITLocal.e12 + _transform.e12 * _intITLocal.e23 + _transform.e13 * _intITLocal.e33;
        Fixed t28 = _transform.e21 * _intITLocal.e11 + _transform.e22 * _intITLocal.e21 + _transform.e23 * _intITLocal.e31;
        Fixed t33 = _transform.e21 * _intITLocal.e12 + _transform.e22 * _intITLocal.e22 + _transform.e23 * _intITLocal.e32;
        Fixed t38 = _transform.e21 * _intITLocal.e12 + _transform.e22 * _intITLocal.e23 + _transform.e23 * _intITLocal.e33;

        Fixed t52 = _transform.e31 * _intITLocal.e11 + _transform.e32 * _intITLocal.e21 + _transform.e33 * _intITLocal.e31;
        Fixed t57 = _transform.e31 * _intITLocal.e12 + _transform.e32 * _intITLocal.e22 + _transform.e33 * _intITLocal.e32;
        Fixed t62 = _transform.e31 * _intITLocal.e12 + _transform.e32 * _intITLocal.e23 + _transform.e33 * _intITLocal.e33;
        _outInvITWorld.e11 = t4 * _transform.e11 + t9 * _transform.e12 + t14 * _transform.e13;
        _outInvITWorld.e12 = t4 * _transform.e21 + t9 * _transform.e22 + t14 * _transform.e23;
        _outInvITWorld.e12 = t4 * _transform.e31 + t9 * _transform.e32 + t14 * _transform.e33;
        _outInvITWorld.e21 = t28 * _transform.e11 + t33 * _transform.e12 + t38 * _transform.e13;
        _outInvITWorld.e22 = t28 * _transform.e21 + t33 * _transform.e22 + t38 * _transform.e23;
        _outInvITWorld.e23 = t28 * _transform.e31 + t33 * _transform.e32 + t38 * _transform.e33;
        _outInvITWorld.e31 = t52 * _transform.e11 + t57 * _transform.e12 + t62 * _transform.e13;
        _outInvITWorld.e32 = t52 * _transform.e21 + t57 * _transform.e22 + t62 * _transform.e23;
        _outInvITWorld.e33 = t52 * _transform.e31 + t57 * _transform.e32 + t62 * _transform.e33;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::CalculateDerivedData( FxTransform& _transform )
    {
        Matrix4 transform( _transform.mRotation, _transform.mPosition );
        TransformInertiaTensor( mInverseInertiaTensorLocal, transform, mInverseInertiaTensorWorld );
        mTransform = &_transform;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::ClearAccumulators()
    {
        mForcesAccumulator = mTorqueAccumulator = Vector3::sZero;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::SetSleeping( const bool _isSleeping )
    {
        if( _isSleeping == mIsSleeping ){ return; }
        if( _isSleeping )
        {
            if( mCanSleep )
            {
                mIsSleeping = true;
                mVelocity   = Vector3::sZero;
                mRotation   = Vector3::sZero;
            }
        }
        else
        {
            mIsSleeping = false;
            mMotion     = 4 * sSleepEpsilon;;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::UpdateMotion()
    {
        Fixed                 newMotion = Vector3::Dot( mVelocity, mVelocity ) + Vector3::Dot( mRotation, mRotation );
        constexpr const Fixed bias      = FIXED( 0.8 );
        mMotion = bias * mMotion + ( 1 - bias ) * newMotion; // recency weighted average
        mMotion = Fixed::Min( mMotion, 10 * sSleepEpsilon );

        if( mMotion < sSleepEpsilon && mCanSleep )
        {
            SetSleeping( true );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Matrix3 FxRigidbody::SphereInertiaTensor( const Fixed _inverseMass, const Fixed _radius )
    {
        const Fixed value = Fixed( 2 ) / 5 * _radius * _radius / _inverseMass;
        return value * Matrix3::sIdentity;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Matrix3 FxRigidbody::BoxInertiaTensor( const Fixed _inverseMass, const Vector3 _halfExtents )
    {
        const Vector3 e      = _halfExtents * 2;
        const Fixed   valueX = Fixed( 1 ) / 12 * ( e.y * e.y + e.z * e.z ) / _inverseMass;
        const Fixed   valueY = Fixed( 1 ) / 12 * ( e.x * e.x + e.z * e.z ) / _inverseMass;
        const Fixed   valueZ = Fixed( 1 ) / 12 * ( e.x * e.x + e.y * e.y ) / _inverseMass;
        return Matrix3( valueX, 0, 0,
                        0, valueY, 0,
                        0, 0, valueZ );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::Save( const EcsComponent& _component, Json& _json )
    {
        const FxRigidbody& rb = static_cast<const FxRigidbody&>( _component );
        Serializable::SaveFixed( _json, "inv_mass", rb.mInverseMass );
        Serializable::SaveVec3( _json, "velocity", rb.mVelocity );
        Serializable::SaveVec3( _json, "rotation", rb.mRotation );
        Serializable::SaveVec3( _json, "acceleration", rb.mAcceleration );
        Serializable::SaveBool( _json, "can_sleep", rb.mCanSleep );
        Serializable::SaveVec3( _json, "inv_inertia_tensor", Vector3( rb.mInverseInertiaTensorLocal.e11, rb.mInverseInertiaTensorLocal.e22, rb.mInverseInertiaTensorLocal.e33 ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FxRigidbody::Load( EcsComponent& _component, const Json& _json )
    {
        FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
        Serializable::LoadFixed( _json, "inv_mass", rb.mInverseMass );
        Serializable::LoadVec3( _json, "velocity", rb.mVelocity );
        Serializable::LoadVec3( _json, "rotation", rb.mRotation );
        Serializable::LoadVec3( _json, "acceleration", rb.mAcceleration );
        Serializable::LoadBool( _json, "can_sleep", rb.mCanSleep );
        Vector3 invInertiaTensor;
        Serializable::LoadVec3( _json, "inv_inertia_tensor", invInertiaTensor );
        rb.mInverseInertiaTensorLocal = Matrix3( invInertiaTensor.x, 0, 0,
                                                 0, invInertiaTensor.y, 0,
                                                 0, 0, invInertiaTensor.z );
    }
}

