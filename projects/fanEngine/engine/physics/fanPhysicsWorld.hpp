#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "engine/physics/fanContactSolver.hpp"
#include "engine/physics/fanCollisionDetection.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Contains all Bullet physics components
    // allows registering of rigidbodies and quick rigidbodies access through handles
    // triggers collision callbacks
    //==================================================================================================================================================================================================
    struct PhysicsWorld : public EcsSingleton
    {
    ECS_SINGLETON( PhysicsWorld )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        Vector3 mGravity;
        Fixed   mLinearDamping; // [0,1] removes the energy added from numerical instability in the integrator
        Fixed   mAngularDamping; // [0,1] removes the energy added from numerical instability in the integrator

        ContactSolver      mContactSolver;
        CollisionDetection mCollisionDetection;
    };
}