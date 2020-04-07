#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include "fanCommandLine.h"

namespace fan
{
	//================================================================================================================================
	// Runs the engine and connects it to the Live++ hot reload
	//================================================================================================================================
	class LPPMain
	{
	public:
		LPPMain( EngineSettings& _settings )
		{
			fan::Engine engine( _settings );
			if( _settings.enableLivepp )
			{
				livePP = lpp::lppLoadAndRegister( L"middleware/LivePP/", "fanEngine" );
				lpp::lppEnableAllCallingModulesSync( livePP );
				engine.onLPPSynch.Connect( &LPPMain::OnSynch, this );
				engine.Run();
				::FreeLibrary( livePP );
			}
			else
			{
				engine.Run();
			}
		}

		void OnSynch() { lpp::lppSyncPoint( livePP ); }

		HMODULE livePP;
	};
}

//==============================================================================================================================================================
// an example command 
// fengine_editor.exe - livepp 0 - scene "content/scenes/game00.scene" - autoplay 1 - window - 1920 0 960 540
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
  args.push_back( "-scene" );
  args.push_back( "content/scenes/game00.scene" );
// 	args.push_back( "-window" );
// 	args.push_back( "1" );
// 	args.push_back( "2" );
// 	args.push_back( "800" );
// 	args.push_back( "600" );
//  args.push_back( "-autoplay" );
//  args.push_back( "1" );

	// Parse the arguments & run the engine
	fan::CommandLine commandLine;
	fan::EngineSettings settings = commandLine.Parse( args );
	fan::LPPMain main( settings );

	return 0;
}