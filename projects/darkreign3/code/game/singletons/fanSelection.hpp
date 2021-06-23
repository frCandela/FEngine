#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/math/fanFixedPoint.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanPrefab.hpp"

namespace fan
{
    class EcsWorld;

    //=================================================================================================
    //=================================================================================================
    struct SelectionStatus
    {
        int  mNumSelected       = 0;
        bool mHoveringOverAlly  = false;
        bool mHoveringOverEnemy = false;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Selection : public EcsSingleton
    {
    ECS_SINGLETON( Selection )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        static SelectionStatus SelectUnits( EcsWorld& _world, const Fixed _delta );

        ResourcePtr<Prefab> mSelectionFramePrefab;
        ResourcePtr<Prefab> mMoveToFxPrefab;
        std::vector<EcsHandle> mSelectionFrames;
    };


}