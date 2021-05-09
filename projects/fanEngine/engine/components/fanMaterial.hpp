#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Rendering parameters form a mesh renderer
    //==================================================================================================================================================================================================
    struct Material : public EcsComponent
    {
    ECS_COMPONENT( Material )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        TexturePtr mTexture;
        uint32_t   mShininess;
        Color      mColor;
    };
}