#include "fanLiveppMain.hpp"

//============================================================================================================
//============================================================================================================
int main( int _argc, char* _argv[] )
{	
	std::vector< std::string > args; // command line arguments
	args.push_back( _argv[0] );

// force arguments into the command line
// 	args.push_back( "-livepp" );
// 	args.push_back( "0" );
//	args.push_back( "-autoplay" );
//	args.push_back( "1" );
// 	args.push_back( "-scene" );
//    args.push_back( "content/scenes/test.scene" );
//	args.push_back( "-editor_client_server" );
//	args.push_back( "-client" );

	// generates a list of strings from the command line arguments
	for( int i = 1; i < _argc; i++ ){	args.push_back( _argv[i] );	}

	// Parse the arguments & run the engine
	fan::LaunchArguments launchArguments;
	fan::LaunchSettings settings = launchArguments.Parse( args );
	fan::LPPMain main( settings ); 

	return 0;
}

int WinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd )
{
    (void)_hInstance;
    (void)_hPrevInstance;
    (void)_lpCmdLine;
    (void)_nShowCmd;

    main( __argc, __argv );
}