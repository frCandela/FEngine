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

        enum class State { Stay, Move };

        // Serialized
        Vector3 mDestination;
        Fixed   mHeightOffset;
        Fixed   mMoveSpeed;     // unit/s
        Fixed   mRotationSpeed; // degrees/s
        State   mState;
        bool    mAlignWithTerrain;

        // runtime only
        Vector3 mTerrainNormal;
        Fixed   mSqrHorizontalDistanceFromDestination;
        Fixed   mForwardAngle;
    };
}