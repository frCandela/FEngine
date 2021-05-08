#include "engine/project/fanLaunchArgs.hpp"

namespace fan
{
    //========================================================================================================
    // registers the default commands
    //========================================================================================================
    LaunchArgs::LaunchArgs()
    {
        RegisterCommand( { &LaunchArgs::CMD_EnableLivePP, "-livepp", "usage: -livepp <0-1>" } );
        RegisterCommand( { &LaunchArgs::CMD_OpenScene, "-scene", "usage: -scene \"scene/path.scene\"" } );
        RegisterCommand( { &LaunchArgs::CMD_SetWindow, "-window", "usage: -window <x> <y> <width> <height>" } );
        RegisterCommand( { &LaunchArgs::CMD_AutoPlay, "-autoplay", "usage: -autoplay <0-1>" } );
    }

    //========================================================================================================
    // returns the command with a name matching _arg
    //========================================================================================================
    const LaunchArgs::LaunchCommand* LaunchArgs::FindCommand( const std::string& _arg )
    {
        for( int cmdIndex = 0; cmdIndex < (int)mCommands.size(); cmdIndex++ )
        {
            const LaunchCommand& cmd = mCommands[cmdIndex];
            if( _arg == cmd.mName )
            {
                return &cmd;
            }
        }
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    bool LaunchArgs::IsCommand( const std::string& _text )
    {
        return FindCommand( _text ) != nullptr;
    }

    //========================================================================================================
    // parse command line arguments and returns EngineSettings
    //========================================================================================================
    void LaunchArgs::Parse( const std::vector<std::string>& _args, LaunchSettings& _settings )
    {
        // parse commands & arguments
        int argIndex = 1;
        while( argIndex < (int)_args.size() )
        {
            const LaunchCommand* command = FindCommand( _args[argIndex] );
            if( command != nullptr )
            {

                // find the arguments of this command
                std::vector<std::string> commandArguments;
                while( ++argIndex < (int)_args.size() && !IsCommand( _args[argIndex] ) )
                {
                    commandArguments.push_back( _args[argIndex] );
                }

                // execute the command
                if( !( *command->mRunCommand )( commandArguments, _settings ) )
                {
                    std::cout << "invalid command line arguments for command " << command->mName << std::endl;
                    std::cout << command->mUsage << std::endl;
                }
            }
            else
            {
                std::cout << "invalid command line" << std::endl;
                break;
            }
        }
    }

    //========================================================================================================
    // command: -livepp <0-1>
    // forces the activation of live++
    //========================================================================================================
    bool LaunchArgs::CMD_EnableLivePP( const std::vector<std::string>& _args, LaunchSettings& _settings )
    {
        if( _args.size() != 1 ){ return false; }

        const int value = std::atoi( _args[0].c_str() );
        if( value != 1 && value != 0 ){ return false; }

        _settings.mEnableLivepp = value == 1 ? true : false;

        std::cout << "cmd : live++ " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;
        return true;
    }

    //========================================================================================================
    // command: -scene "scene/path.scene"
    // auto opens a scene at startup
    //========================================================================================================
    bool LaunchArgs::CMD_OpenScene( const std::vector<std::string>& _args, LaunchSettings& _settings )
    {
        if( _args.size() != 1 ){ return false; }

        _settings.mLoadScene = _args[0];

        std::cout << "cmd : open scene " << _settings.mLoadScene << std::endl;
        return true;
    }

    //========================================================================================================
    // command: -window <x <y> <width> <height>"
    // moves the engine window it position (x,y) and resizes it to (width,height)
    //========================================================================================================
    bool LaunchArgs::CMD_SetWindow( const std::vector<std::string>& _args, LaunchSettings& _settings )
    {
        if( _args.size() != 4 ){ return false; }

        _settings.mWindow_position.x     = std::atoi( _args[0].c_str() );
        _settings.mWindow_position.y     = std::atoi( _args[1].c_str() );
        _settings.mWindow_size.x         = std::atoi( _args[2].c_str() );
        _settings.mWindow_size.y         = std::atoi( _args[3].c_str() );
        _settings.mForceWindowDimensions = true;

        std::cout << "cmd : window position(" << _settings.mWindow_position.x
                << "," << _settings.mWindow_position.y << ")";
        std::cout << ", size(" << _settings.mWindow_size.x
                << "," << _settings.mWindow_size.y << ")" << std::endl;
        return true;
    }

    //========================================================================================================
    // command: -autoplay <0-1>"
    // if a scene is loaded at startup, plays it directly
    //========================================================================================================
    bool LaunchArgs::CMD_AutoPlay( const std::vector<std::string>& _args,
                                   LaunchSettings& _settings )
    {
        if( _args.size() != 1 ){ return false; }

        const int value = std::atoi( _args[0].c_str() );
        if( value != 1 && value != 0 ){ return false; }

        _settings.mAutoPlay = value == 1 ? true : false;

        std::cout << "cmd : autoplay " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;
        return true;
    }
}