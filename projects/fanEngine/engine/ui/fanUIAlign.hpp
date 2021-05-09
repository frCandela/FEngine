#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
    struct UITransform;

    //==================================================================================================================================================================================================
    // aligns the ui transform of this entity with the one on the parent entity
    // see SAlignUI
    //==================================================================================================================================================================================================
    struct UIAlign : public EcsComponent
    {
    ECS_COMPONENT( UIAlign )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        enum AlignCorner : int
        {
            TopLeft, TopRight, BottomLeft, BottomRight
        };
        enum AlignDirection : int
        {
            Horizontal, Vertical, HorizontalVertical
        };
        enum UnitType : int
        {
            Ratio, Pixels
        };

        AlignCorner mCorner;
        AlignDirection mDirection;
        UnitType mUnitType;
        glm::vec2 mOffset; // in pixel or in size ratio
    };
}