#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanVector3.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "render/resources/fanAnimation.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Judas : public EcsComponent
    {
    ECS_COMPONENT( Judas )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void PostInit( EcsWorld& _world, EcsEntity _entity );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        static void OnFire( EcsWorld& _world, EcsEntity _unitEntity );
        static void OnDeath( EcsWorld& _world, EcsEntity _unitEntity );

        ResourcePtr <Animation> mAnimIdle;
        ResourcePtr <Animation> mAnimWalk;
        ResourcePtr <Animation> mAnimRun;
        ResourcePtr <Animation> mAnimFire;
        ResourcePtr <Prefab>    mFireFx;
        ResourcePtr <Prefab>    mDeathFx;
    };
}