#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    struct FxRigidbody;
    struct FxTransform;

    //========================================================================================================
    // Contains all Bullet physics components
    // allows registering of rigidbodies and quick rigidbodies access through handles
    // triggers collision callbacks
    //========================================================================================================
    struct FxPhysicsWorld : public EcsSingleton
    {
    ECS_SINGLETON( FxPhysicsWorld )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        struct Contact
        {
            FxRigidbody* rb0;
            FxRigidbody* rb1;
            FxTransform* transform0;
            FxTransform* transform1;
            Vector3 normal;
            Fixed   restitution;
            Fixed   penetration;
        };

        Vector3 mGravity;
        Fixed   mDamping; // [0,1] removes the energy added from numerical instability in the integrator

        static void ResolveContact( const Contact& _contact, Fixed _duration );
        static Fixed CalculateSeparatingVelocity( const Contact& _contact );
        static void ResolveVelocity( const Contact& _contact, Fixed _duration );
        static void ResolveInterpenetration( const Contact& _contact, Fixed _duration );
    };
}