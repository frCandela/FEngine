#pragma once

#include "core/ecs/fanEcsTag.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // scene tags
    //==================================================================================================================================================================================================
    struct TagEditorOnly : EcsTag
    {
    ECS_TAG( TagEditorOnly )
    };        // entity is not saved
    struct TagUIModified : EcsTag
    {
    ECS_TAG( TagUIModified )
    };
    struct TagUIEnabled : EcsTag
    {
    ECS_TAG( TagUIEnabled )
    };
    struct TagUIVisible : EcsTag
    {
    ECS_TAG( TagUIVisible )
    };
}