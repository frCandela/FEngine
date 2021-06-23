#pragma once

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct TestSingleton : public EcsSingleton
    {
    ECS_SINGLETON( TestSingleton )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        float mValue;
    };
}