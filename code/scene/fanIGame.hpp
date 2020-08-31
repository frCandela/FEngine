#pragma once

#include "ecs/fanSignal.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class IGame
    {
    public:
        IGame(){}
        IGame( IGame const& ) = delete;
        IGame& operator=( IGame const& ) = delete;

        virtual void Init() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Step( const float _delta ) = 0;

        EcsWorld    mWorld;
        std::string mName;

        // called by the editor
        Signal<> mOnSwitchToGameCamera;
    };
}