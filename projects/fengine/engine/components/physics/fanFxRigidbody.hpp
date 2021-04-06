#pragma  once

#include "core/math/fanVector3.hpp"
#include "core/ecs/fanEcsComponent.hpp"

namespace fan
{
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
        Vector3 mVelocity;
        Vector3 mAcceleration; // constant forces

        Vector3 mForcesAccumulator;

        void ApplyForce(const Vector3& _force ){ mForcesAccumulator += _force; }
    };
}