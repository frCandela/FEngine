#pragma once

#include "engine/game/fanIGame.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DarkReign3 : IGame
    {
        void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const Fixed _delta ) override;
        void Render() override;
        void OnGui() override;
        EcsWorld& GetWorld() override { return mWorld; }

        void StepLoadTerrain();

        EcsWorld mWorld;
    };
}
