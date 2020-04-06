#pragma once

#include "editor/fanEngine.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// CommandLine generates EngineSettings from command line arguments 
	//==============================================================================================================================================================
	struct CommandLine
	{
		//===============================================================================
		//===============================================================================
		struct Command
		{
			bool ( *Run )( const std::vector < std::string >& _args, EngineSettings& _settings ) = nullptr; // run the command ( parse arguments & modify the settings )
			std::string name = "";																			// command string
			std::string usage = "";																			// usage helper string
		};

		CommandLine();
		EngineSettings	Parse( const std::vector< std::string >& _args );
		const Command*  FindCommand( const std::string& _name );										  // returns the command matching _name
		bool			IsCommand( const std::string& _text ) { return FindCommand( _text ) != nullptr; } // returns true it _text matches a command

		std::vector< Command >	commands;	// the full list of commands of the engine

		// commands
		static bool CMD_EnableLivePP( const std::vector < std::string >& _args, EngineSettings& _settings );
	};

	//==============================================================================================================================================================
	// generates the list of all commands
	//==============================================================================================================================================================
	CommandLine::CommandLine() :
		commands( {
			{ &CommandLine::CMD_EnableLivePP, "-livepp", "usage: livepp <0-1>" }
			} )
	{}

	//==============================================================================================================================================================
	// returns the command with a name matching _arg
	//==============================================================================================================================================================
	const CommandLine::Command* CommandLine::FindCommand( const std::string& _arg )
	{
		for( int cmdIndex = 0; cmdIndex < commands.size(); cmdIndex++ )
		{
			const Command& cmd = commands[cmdIndex];
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
	EngineSettings CommandLine::Parse( const std::vector< std::string >& _args )
	{
		EngineSettings settings;

		// live++ setting
#ifndef NDEBUG		
		settings.enableLivepp = true;
#else				
		settings.enableLivepp = false;
#endif // !NDEBUG

		// parse arguments looking for commands
		int argIndex = 1;
		while( argIndex < _args.size() )
		{
			// find the command		 
			const Command* command = FindCommand( _args[argIndex] );
			if( command != nullptr )
			{

				// find the arguments
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
				// error
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
	bool CommandLine::CMD_EnableLivePP( const std::vector < std::string >& _args, EngineSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		const int value = std::atoi( _args[0].c_str() );
		if( value != 1 && value != 0 ) { return false; }

		_settings.enableLivepp = value == 1 ? true : false;

		std::cout << "cmd : live++ " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;

		return true;
	}
}