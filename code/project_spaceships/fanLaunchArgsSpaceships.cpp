#include "project_spaceships/fanLaunchArgsSpaceships.hpp"

namespace fan
{
    LaunchArgsSpaceships::LaunchArgsSpaceships()
    {
        RegisterCommand( { &LaunchArgsSpaceships::CMD_RunClient,
                           "-client",
                           "usage: -client" } );

        RegisterCommand( { &LaunchArgsSpaceships::CMD_RunServer,
                           "-server",
                           "usage: -server" } );
    }



    //========================================================================================================
	// command: -client"
	// runs the game in client mode with the editor
	//========================================================================================================
    bool LaunchArgsSpaceships::CMD_RunClient( const std::vector<std::string>& _args,
                                                    LaunchSettings& _settings )
	{
		if( _args.size() != 0 ) { return false; }


        LaunchSettingsSpaceships& settings = static_cast<LaunchSettingsSpaceships&>( _settings );
        settings.mNetworkMode = LaunchSettingsSpaceships::NetworkMode::Client;

		std::cout << "cmd : launch editor client" << std::endl;
		return true;
	}

    //========================================================================================================
    // command: -server"
    // runs the game in server mode with the editor
    //========================================================================================================
    bool LaunchArgsSpaceships::CMD_RunServer( const std::vector<std::string>& _args,
                                                    LaunchSettings& _settings )
    {
        if( _args.size() != 0 ) { return false; }

        LaunchSettingsSpaceships& settings = static_cast<LaunchSettingsSpaceships&>( _settings );
        settings.mNetworkMode = LaunchSettingsSpaceships::NetworkMode::Server;

        std::cout << "cmd : launch editor server" << std::endl;
        return true;
    }
}