#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct TerrainAgent : public EcsComponent
    {
    ECS_COMPONENT( TerrainAgent )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        bool TargetIsInRange() const { return mSqrDistanceFromTarget < mRange * mRange; }

        enum State
        {
            Stay, // do nothing
            Move, // move towards the target position
            Face  // rotate to face the target position
        };

        // serialized
        Fixed mHeightOffset;
        Fixed mMaxMoveSpeed;     // unit/s
        Fixed mMaxRotationSpeed; // degrees/s
        Fixed mRange;         // distance of target at which the unit will stop moving
        bool  mAlignWithTerrain;

        // runtime only
        State   mState;
        Vector3 mTargetPosition;
        Vector3 mPositionOnTerrain;
        Vector3 mTerrainNormal;
        Fixed   mSqrDistanceFromTarget;
        Fixed   mForwardAngle;
    };
}