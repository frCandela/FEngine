#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include "fanLaunchArguments.h"

namespace fan
{
	//================================================================================================================================
	// Runs the engine and connects it to the Live++ hot reload
	//================================================================================================================================
	class LPPMain
	{
	public:
		LPPMain( LaunchSettings& _settings )
		{
			if( _settings.launchServer )
			{
				// attaches an editor to a game server and runs it
				fan::GameServer server( "server" );
				fan::Editor editor( _settings, server.world );
				RunEditor( _settings, editor );
			}
			else
			{
				// attaches an editor to a game client and runs it
				fan::GameClient client( "client" );
				fan::Editor editor( _settings, client.world );
				RunEditor( _settings, editor );
			}
		}

		void RunEditor( LaunchSettings& _settings, fan::Editor& _editor )
		{
			if( _settings.enableLivepp )
			{
				livePP = lpp::lppLoadAndRegister( L"middleware/LivePP/", "fanEngine" );
				lpp::lppEnableAllCallingModulesSync( livePP );
				_editor.onLPPSynch.Connect( &LPPMain::OnSynch, this );
				_editor.Run();
				::FreeLibrary( livePP );
			}
			else
			{
				_editor.Run();
			}
		}

		void OnSynch() { lpp::lppSyncPoint( livePP ); }

		HMODULE livePP;
	};
}

//==============================================================================================================================================================
//==============================================================================================================================================================
int main( int _argc, char* _argv[] )
{
	// generates a list of strings from the command line arguments
	std::vector< std::string > args;
	for( int i = 0; i < _argc; i++ )
	{ 
		args.push_back( _argv[i] ); 
	}

// force arguments into the command line
// 	args.push_back( "-livepp" );
// 	args.push_back( "0" );
//  args.push_back( "-scene" );
//  args.push_back( "content/scenes/game00.scene" );
// 	args.push_back( "-window" );
// 	args.push_back( "1" );
// 	args.push_back( "2" );
// 	args.push_back( "800" );
// 	args.push_back( "600" );
//  args.push_back( "-autoplay" );
//  args.push_back( "1" );

	// Parse the arguments & run the engine
	fan::LaunchArguments commandLine;
	fan::LaunchSettings settings = commandLine.Parse( args );
	fan::LPPMain main( settings );

	return 0;
}