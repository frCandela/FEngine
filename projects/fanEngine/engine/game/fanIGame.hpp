#pragma once

#include "core/fanSignal.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct IGame
    {
        virtual ~IGame() {}
        virtual void Init() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void PreStep( const Fixed _delta ) = 0;     // called immediately before physics step
        virtual void Step( const Fixed _delta ) = 0;        // called after physics step
        virtual void OnGui() = 0;
        virtual EcsWorld& GetWorld() = 0;
    };
}