#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"
#include "core/math/fanMatrix3.hpp"
#include "core/math/fanMatrix4.hpp"
#include "engine/fanSceneResourcePtr.hpp"

namespace fan
{
    struct FxTransform;

    //========================================================================================================
    //========================================================================================================
    struct FxRigidbody : public EcsComponent
    {
        ECS_COMPONENT( FxRigidbody )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        Fixed   mInverseMass;
        Matrix3 mInverseInertiaTensorLocal;
        Vector3 mVelocity;
        Vector3 mRotation;
        Vector3 mAcceleration; // constant forces

        Vector3 mForcesAccumulator;
        Vector3 mTorqueAccumulator;
        Matrix3 mInverseInertiaTensorWorld;
        FxTransform* mTransform; // buffered at the beginning of the frame

        void ApplyCentralForce( const Vector3& _force ) { mForcesAccumulator += _force; }
        void ApplyForce( const Vector3& _force, const Vector3& _localPoint );
        void ApplyTorque( const Vector3& _torque ) { mTorqueAccumulator += _torque; }
        void CalculateDerivedData( FxTransform& _transform );
        void ClearAccumulators();

        static Matrix3 SphereInertiaTensor( const Fixed _inverseMass, const Fixed _radius );
        static Matrix3 BoxInertiaTensor( const Fixed _inverseMass, const Vector3 _halfExtents );
    };
}