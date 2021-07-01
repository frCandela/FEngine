#pragma once

#include "engine/game/fanIGame.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GameEmpty : IGame
    {
        void Init() override;
        void Start() override;
        void Stop() override;
        void PreStep( const Fixed _delta ) override;
        void Step( const Fixed _delta ) override;
        void Render() override;
        void OnGui() override;
        EcsWorld& GetWorld() override { return mWorld; }

        EcsWorld mWorld;
    };
}
