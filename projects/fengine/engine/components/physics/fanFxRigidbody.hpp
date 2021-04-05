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

        static const constexpr Fixed sGravitation = FIXED(9.807);
        static const constexpr Fixed sDamping = FIXED( 1 ); // [0,1] removes the energy added from numerical instability in the integrator

        Fixed   mInverseMass;
        Vector3 mVelocity;
        Vector3 mAcceleration;
    };
}