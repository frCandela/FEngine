#pragma once

#include "ecs/fanEcsWorld.hpp"
#include "engine/game/fanPlayerData.hpp"
#include "network/fanNetConfig.hpp"

namespace fan
{
    class Window;
    struct Scene;

    //==================================================================================================================================================================================================
    // This container runs the game
    // Auto load a scene & displays it, no editor features
    // runs the main loop & manages events
    //==================================================================================================================================================================================================
    struct GamePlayer
    {
        GamePlayer( LaunchSettings& _settings, IGame& _game );

        void Run();
        void Step();
        void Exit() { mData.mApplicationShouldExit = true; };

        IGame& mGame;
        PlayerData mData;
        FrameIndex mLastLogicFrameRendered;
        LaunchSettings& AdaptSettings( LaunchSettings& _settings );
    };
}