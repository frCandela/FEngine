#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanTexture.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UIRenderer : public EcsComponent
    {
    ECS_COMPONENT( UIRenderer )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        ResourcePtr<Mesh2D>  mMesh2D;
        Color                mColor;
        glm::ivec2           mTileIndex;
        glm::ivec2           mTiling;
        int                  mDepth;
        ResourcePtr<Texture> mTexture;

        glm::ivec2 GetTextureSize() const;
    };
}