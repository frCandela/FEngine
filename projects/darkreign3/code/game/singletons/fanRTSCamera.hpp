#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RTSCamera : public EcsSingleton
    {
    ECS_SINGLETON( RTSCamera )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        static void CreateCamera( EcsWorld& _world );
        static void Update( EcsWorld& _world, const Fixed _delta );

        EcsHandle mCameraHandle;
        Fixed mMaxHeight;
        Fixed mMinHeight;
        Fixed mZoomSpeed;
        Fixed mTranslationSpeed;
    };
}