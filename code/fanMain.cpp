#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include "fanLaunchArguments.h"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"
#include "scene/fanGameHolder.hpp"
#include "editor/fanEditorHolder.hpp"


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
			// enables livepp
			if( _settings.enableLivepp )
			{
				m_livePP = lpp::lppLoadAndRegister( L"middleware/LivePP/", "fanEngine" );
				lpp::lppEnableAllCallingModulesSync( m_livePP );
			}

			// runs the game
			switch( _settings.launchMode )
			{
			case LaunchSettings::Mode::EditorClient:
			{
				_settings.windowName = _settings.windowName + "_editor_client";

				// attaches an editor to a game client and runs it
				fan::GameClient client;
				fan::EditorHolder editor( _settings, { &client } );
				if( _settings.enableLivepp )
				{
					editor.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
				}
				editor.Run();
			} 
			break;
			case LaunchSettings::Mode::EditorServer:
			{
				_settings.windowName = _settings.windowName + "_editor_server";

				// attaches an editor to a game server and runs it
				fan::GameServer server;
				fan::EditorHolder editor( _settings, { &server } );
				if( _settings.enableLivepp )
				{
					editor.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
				}
				editor.Run();
			} 
			break;
			case LaunchSettings::Mode::EditorClientServer:
			{
				_settings.windowName = _settings.windowName + "_editor_client_server";

				// attaches an editor to a game client and runs it
				fan::GameClient client;
				fan::GameServer server;
				fan::EditorHolder editor( _settings, { &client, &server } );
				if( _settings.enableLivepp )
				{
					editor.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
				}
				editor.Run();
			} break;
			case LaunchSettings::Mode::Client:
			{
				_settings.windowName = _settings.windowName + "_client";

				// attaches an editor to a game client and runs it
				fan::GameClient client;
				fan::GameHolder gameHolder( _settings, client );
				if( _settings.enableLivepp )
				{
					gameHolder.onLPPSynch.Connect( &LPPMain::OnSync, this );
				}
				gameHolder.Run();
			} break;
			case LaunchSettings::Mode::Server:
			{
				_settings.windowName = _settings.windowName + "_server";

				// attaches an editor to a game client and runs it
				fan::GameServer server;
				fan::GameHolder gameHolder( _settings, server );
				if( _settings.enableLivepp )
				{
					gameHolder.onLPPSynch.Connect( &LPPMain::OnSync, this );
				}
				gameHolder.Run();
			} break;
			default:
				assert( false );
				break;
			}

			// disables livepp
			if( _settings.enableLivepp )
			{
				::FreeLibrary( m_livePP );
			}
		}

	private:
		void OnSync() { lpp::lppSyncPoint( m_livePP ); }

		HMODULE m_livePP;
	};
}

//==============================================================================================================================================================
//==============================================================================================================================================================
int main( int _argc, char* _argv[] )
{	
	std::vector< std::string > args; // command line arguments
	args.push_back( _argv[0] );

// force arguments into the command line
// 	args.push_back( "-livepp" );
// 	args.push_back( "0" );
// 	args.push_back( "-window" );
// 	args.push_back( "1" );
// 	args.push_back( "2" );
// 	args.push_back( "800" );
// 	args.push_back( "600" );
// 	args.push_back( "-main_loop_sleep" );
// 	args.push_back( "1" );
//	args.push_back( "-autoplay" );
//	args.push_back( "1" );
 	args.push_back( "-scene" );
    args.push_back( "content/scenes/test.scene" );
	args.push_back( "-editor_client_server" );
//	args.push_back( "-client" );

	// generates a list of strings from the command line arguments
	for( int i = 1; i < _argc; i++ ){	args.push_back( _argv[i] );	}

	// Parse the arguments & run the engine
	fan::LaunchArguments launchArguments;
	fan::LaunchSettings settings = launchArguments.Parse( args );
	fan::LPPMain main( settings ); 

	return 0;
}