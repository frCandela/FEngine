#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "fanGlm.hpp"

namespace fan
{
    struct UITransform;

    //==================================================================================================================================================================================================
    // aligns all child ui transforms with their brothers and with this entity ui transform
    // see SUpdateUILayouts
    //==================================================================================================================================================================================================
    struct UILayout : public EcsComponent
    {
    ECS_COMPONENT( UILayout )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        enum Type : int
        {
            Horizontal, Vertical, Grid
        };

        Type       mType;
        glm::ivec2 mGap;
        bool       mFill;
    };
}