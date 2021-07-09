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

        enum State
        {
            Stay, Move
        };

        // Serialized
        Fixed mHeightOffset;
        Fixed mMoveSpeed;     // unit/s
        Fixed mRotationSpeed; // degrees/s
        Fixed mRange;           // distance of target at which the unit will stop moving
        bool  mAlignWithTerrain;

        // runtime only
        State   mState;
        Vector3 mDestination;
        Vector3 mTerrainPosition;
        Vector3 mTerrainNormal;
        Fixed   mSqrDistanceFromDestination;
        Fixed   mForwardAngle;
    };
}