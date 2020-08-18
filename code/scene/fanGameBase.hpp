#pragma once

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class GameBase
    {
    public:
        GameBase(){}
        GameBase( GameBase const& ) = delete;
        GameBase& operator=( GameBase const& ) = delete;

        virtual void Init() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Step( const float _delta ) = 0;

        EcsWorld mWorld;
    };
}