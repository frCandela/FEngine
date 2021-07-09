#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    struct UnitOrder
    {
        enum Type
        {
            None,
            Attack,
            Move
        };
        Type    mType = Type::None;
        bool    mWasExecuted = false;
        Vector3 mPosition;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Unit : public EcsComponent
    {
    ECS_COMPONENT( Unit )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        enum State
        {
            Wait,
            Move,
            Attack,
        };

        Fixed mAttackRange;

        // non serialized
        State     mState;
        UnitOrder mLastOrder;
    };
}