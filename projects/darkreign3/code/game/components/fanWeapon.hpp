#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    class Prefab;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Weapon : public EcsComponent
    {
    ECS_COMPONENT( Weapon )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        static void FireLaser(EcsWorld& _world, Prefab& _prefab, const Vector3& _origin, const Vector3& _target );

        Fixed mDamage;      // hp
        Fixed mRateOfFire;  // round/s

        // non serialized
        Fixed mTimeAccumulator;
    };
}