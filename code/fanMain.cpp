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
//==============================================================================================================================================================
int main( int _argc, char* _argv[] )
{
	// generates a list of strings from the command line arguments
	std::vector< std::string > args;
	for( int i = 0; i < _argc; i++ ) { args.push_back( _argv[i] ); }

	// 	args.push_back( "-livepp" );
	// 	args.push_back( "0" );

		// Parse the arguments & rune the engine
	fan::CommandLine commandLine;
	fan::EngineSettings settings = commandLine.Parse( args );
	fan::LPPMain main( settings );

	return 0;
}