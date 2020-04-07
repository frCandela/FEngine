#pragma once

#include "editor/fanEngine.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// CommandLine generates EngineSettings from command line arguments 
	//==============================================================================================================================================================
	struct LaunchArguments
	{
		//===============================================================================
		//===============================================================================
		struct LaunchCommand
		{
			bool ( *Run )( const std::vector < std::string >& _args, LaunchSettings& _settings ) = nullptr; // run the command ( parse arguments & modify the settings )
			std::string name = "";																			// command string
			std::string usage = "";																			// usage helper string
		};

		LaunchArguments();
		LaunchSettings			Parse( const std::vector< std::string >& _args );								  // parses the command line & returns its settings
		const LaunchCommand*	FindCommand( const std::string& _name );										  // returns the command matching _name
		bool					IsCommand( const std::string& _text ) { return FindCommand( _text ) != nullptr; } // returns true it _text matches a command

		std::vector< LaunchCommand >	commands;	// the full list of commands of the engine

		// commands
		static bool CMD_EnableLivePP( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_OpenScene( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_SetWindow( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_AutoPlay( const std::vector < std::string >& _args, LaunchSettings& _settings );
	};

	//==============================================================================================================================================================
	// generates the list of all commands
	//==============================================================================================================================================================
	LaunchArguments::LaunchArguments() :
		commands( {
			{ &LaunchArguments::CMD_EnableLivePP,	"-livepp",	 "usage: -livepp <0-1>" },
			{ &LaunchArguments::CMD_OpenScene,		"-scene",	 "usage: -scene \"scene/path.scene\"" },
			{ &LaunchArguments::CMD_SetWindow,		"-window",	 "usage:  -window <x> <y> <width> <height>" },
			{ &LaunchArguments::CMD_AutoPlay,		"-autoplay", "usage:  -autoplay <0-1>" }
		} )
	{}

	//==============================================================================================================================================================
	// returns the command with a name matching _arg
	//==============================================================================================================================================================
	const LaunchArguments::LaunchCommand* LaunchArguments::FindCommand( const std::string& _arg )
	{
		for( int cmdIndex = 0; cmdIndex < commands.size(); cmdIndex++ )
		{
			const LaunchCommand& cmd = commands[cmdIndex];
			if( _arg == cmd.name )
			{
				return &cmd;
			}
		}
		return nullptr;
	}

	//==============================================================================================================================================================
	// parse command line arguments and returns EngineSettings
	//==============================================================================================================================================================
	LaunchSettings LaunchArguments::Parse( const std::vector< std::string >& _args )
	{
		LaunchSettings settings;

		// live++ default settings
#ifndef NDEBUG		
		settings.enableLivepp = true;
#else				
		settings.enableLivepp = false;
#endif // !NDEBUG

		// parse commands & arguments
		int argIndex = 1;
		while( argIndex < _args.size() )
		{
			const LaunchCommand* command = FindCommand( _args[argIndex] );
			if( command != nullptr )
			{

				// find the arguments of this command
				std::vector < std::string > commandArguments;
				while( ++argIndex < _args.size() && !IsCommand( _args[argIndex] ) )
				{
					commandArguments.push_back( _args[argIndex] );
				}

				// execute the command
				if( !( *command->Run )( commandArguments, settings ) )
				{
					std::cout << "invalid command line arguments for command " << command->name << std::endl;
					std::cout << command->usage << std::endl;
				}
			}
			else
			{
				std::cout << "invalid command line" << std::endl;
				break;
			}
		}
		return settings;
	}

	//==============================================================================================================================================================
	// command: -livepp <0-1>
	// forces the activation of live++ 
	//==============================================================================================================================================================
	bool LaunchArguments::CMD_EnableLivePP( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		const int value = std::atoi( _args[0].c_str() );
		if( value != 1 && value != 0 ) { return false; }

		_settings.enableLivepp = value == 1 ? true : false;

		std::cout << "cmd : live++ " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;

		return true;
	}

	//==============================================================================================================================================================
	// command: -scene "scene/path.scene"
	// auto opens a scene at startup
	//==============================================================================================================================================================
	bool LaunchArguments::CMD_OpenScene( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		_settings.loadScene = _args[0];

		std::cout << "cmd : open scene " << _settings.loadScene << std::endl;
		return true;
	}

	//==============================================================================================================================================================
	// command: -window <x <y> <width> <height>"
	// moves the engine window it position (x,y) and resizes it to (width,height)
	//==============================================================================================================================================================
	bool LaunchArguments::CMD_SetWindow( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 4 ) { return false; }

		_settings.window_position.x = std::atoi( _args[0].c_str() );
		_settings.window_position.y = std::atoi( _args[1].c_str() );
		_settings.window_size.x = std::atoi( _args[2].c_str() );
		_settings.window_size.y = std::atoi( _args[3].c_str() );

		std::cout << "cmd : window position(" << _settings.window_position.x << "," << _settings.window_position.y << ")";
		std::cout << ", size(" << _settings.window_size.x << "," << _settings.window_size.y << ")" << std::endl;
		return true;
	}

	//==============================================================================================================================================================
	// command: -autoplay <0-1>"
	// if a scene is loaded at startup, plays it directly
	//==============================================================================================================================================================
	bool LaunchArguments::CMD_AutoPlay( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		const int value = std::atoi( _args[0].c_str() );
		if( value != 1 && value != 0 ) { return false; }

		_settings.autoPlay = value == 1 ? true : false;

		std::cout << "cmd : autoplay " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;

		return true;
	}
}