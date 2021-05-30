#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanPrefab.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Selection : public EcsSingleton
    {
    ECS_SINGLETON( Selection )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        ResourcePtr<Prefab> mSelectionFramePrefab;
        std::vector<EcsHandle> mSelectionFrames;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SelectionStatus
    {
        int  mNumSelected       = 0;
        bool mHoveringOverAlly  = false;
        bool mHoveringOverEnemy = false;
    };
}