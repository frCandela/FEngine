#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "render/resources/fanAnimation.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Animator : public EcsComponent
    {
    ECS_COMPONENT( Animator )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        ResourcePtr<Animation> mAnimation;
        int mKeyframe = 0;
    };
}