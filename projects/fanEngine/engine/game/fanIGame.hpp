#pragma once

#include "ecs/fanSignal.hpp"
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
        virtual void Step( const Fixed _delta ) = 0;
        virtual void Render() = 0;
        virtual void OnGui() = 0;
        virtual EcsWorld& GetWorld() = 0;
    };
}