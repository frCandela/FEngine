#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"
#include "core/math/fanMatrix3.hpp"
#include "core/math/fanMatrix4.hpp"
#include "engine/resources/fanComponentPtr.hpp"

namespace fan
{
    struct Transform;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Rigidbody : public EcsComponent
    {
    ECS_COMPONENT( Rigidbody )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        Fixed   mInverseMass;
        Matrix3 mInverseInertiaTensorLocal;
        Vector3 mVelocity;
        Vector3 mRotation;
        Vector3 mAcceleration; // constant forces
        bool    mCanSleep;

        static const constexpr Fixed sSleepEpsilon = FIXED( 0.031 );
        bool                         mIsSleeping;
        Fixed                        mMotion;
        Vector3                      mForcesAccumulator;
        Vector3                      mTorqueAccumulator;
        Matrix3                      mInverseInertiaTensorWorld;
        Transform* mTransform; // buffered at the beginning of the frame

        void ApplyCentralForce( const Vector3& _force ) { mForcesAccumulator += _force; }
        void ApplyForce( const Vector3& _force, const Vector3& _localPoint );
        void ApplyTorque( const Vector3& _torque ) { mTorqueAccumulator += _torque; }
        void CalculateDerivedData( Transform& _transform );
        void ClearAccumulators();
        void SetSleeping( const bool _isSleeping );
        void UpdateMotion();

        static Matrix3 SphereInertiaTensor( const Fixed _inverseMass, const Fixed _radius );
        static Matrix3 BoxInertiaTensor( const Fixed _inverseMass, const Vector3 _halfExtents );
    };
}