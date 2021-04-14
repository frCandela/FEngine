#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct FxSphereCollider : public EcsComponent
    {
        ECS_COMPONENT( FxSphereCollider )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        Fixed mRadius;
        Vector3 mOffset;
    };
}