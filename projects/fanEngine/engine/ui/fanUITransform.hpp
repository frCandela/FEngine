#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UITransform : public EcsComponent
    {
    ECS_COMPONENT( UITransform )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        glm::ivec2 mPosition;
        glm::ivec2 mSize;
    };
}