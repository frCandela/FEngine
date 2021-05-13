#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "fanUITransform.hpp"
#include "engine/fanSceneResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UIProgressBar : public EcsComponent
    {
    ECS_COMPONENT( UIProgressBar )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        ComponentPtr <UITransform> mTargetTransform;
        float                      mProgress; // between 0-1
        int                        mMaxSize;  // in pixels

        void SetProgress( const float _progress );
    };
}