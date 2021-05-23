#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanFont.hpp"

namespace fan
{
    struct Mesh2D;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UIText : public EcsComponent
    {
    ECS_COMPONENT( UIText )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        std::string mText;
        ResourcePtr<Mesh2D> mMesh2D;
        ResourcePtr<Font> mFontPtr;
        int     mSize;
    };
}