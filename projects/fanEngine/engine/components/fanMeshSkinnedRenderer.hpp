#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "render/resources/fanMeshSkinned.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // allows rendering of a skinned mesh
    // needs a transform and a material
    //==================================================================================================================================================================================================
    struct MeshSkinnedRenderer : public EcsComponent
    {
    ECS_COMPONENT( MeshSkinnedRenderer )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        ResourcePtr<MeshSkinned> mMesh;
    };
}