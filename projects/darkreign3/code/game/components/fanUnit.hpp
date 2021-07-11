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
        Type      mType         = Type::None;
        bool      mWasExecuted  = false;
        Vector3   mTargetPosition;
        EcsHandle mTargetEntity = 0;
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

        bool HasTarget() const { return mLastOrder.mTargetEntity != 0; }

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