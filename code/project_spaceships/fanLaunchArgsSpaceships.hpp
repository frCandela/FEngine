#pragma once

#include "engine/project/fanLaunchArgs.hpp"
#include "engine/project/fanLaunchSettings.hpp"


namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct LaunchSettingsSpaceships : LaunchSettings
    {
        enum class NetworkMode { Client, Server };

        NetworkMode mNetworkMode = NetworkMode::Client; // launch server/client & game/editor
    };

    //========================================================================================================
    //========================================================================================================
    class LaunchArgsSpaceships : public LaunchArgs
    {
    public:
        LaunchArgsSpaceships();

    private:
        using ArgsList = const std::vector < std::string >&;

        static bool CMD_RunClient( ArgsList _args, LaunchSettings& _settings );
        static bool CMD_RunServer( ArgsList _args, LaunchSettings& _settings );
    };
}