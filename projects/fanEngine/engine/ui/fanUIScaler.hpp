#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // makes a transform cover the combined bounds of it's child transforms
    //==================================================================================================================================================================================================
    struct UIScaler : public EcsComponent
    {
    ECS_COMPONENT( UIScaler )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        enum ScaleDirection : int
        {
            Horizontal, Vertical, HorizontalVertical
        };

        ScaleDirection mScaleDirection;
    };
}