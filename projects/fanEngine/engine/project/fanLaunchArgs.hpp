#pragma once

#include <vector>
#include <iostream>
#include "engine/project/fanLaunchSettings.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // CommandLine generates LaunchSettings from program launch arguments
    //==================================================================================================================================================================================================
    class LaunchArgs
    {
    public:
        //===============================================================================
        //===============================================================================
        struct LaunchCommand
        {
            using Command = bool ( * )( const std::vector<std::string>& _args, LaunchSettings& _settings );
            Command     mRunCommand = nullptr; // run the command ( parse arguments & modify the settings )
            std::string mName       = "";    // command string
            std::string mUsage      = "";    // usage helper string
        };

        LaunchArgs();
        void Parse( const std::vector<std::string>& _args, LaunchSettings& _settings );

    protected:
        inline void RegisterCommand( const LaunchCommand& _command ) { mCommands.push_back( _command ); }

    private:
        const LaunchCommand* FindCommand( const std::string& _name );
        bool IsCommand( const std::string& _text );

        std::vector<LaunchCommand> mCommands;

        // default launch commands
        static bool CMD_EnableLivePP( const std::vector<std::string>& _args, LaunchSettings& _settings );
        static bool CMD_OpenScene( const std::vector<std::string>& _args, LaunchSettings& _settings );
        static bool CMD_SetWindow( const std::vector<std::string>& _args, LaunchSettings& _settings );
        static bool CMD_AutoPlay( const std::vector<std::string>& _args, LaunchSettings& _settings );
        static bool CMD_MainLoopSleep( const std::vector<std::string>& _args, LaunchSettings& _settings );
    };
}