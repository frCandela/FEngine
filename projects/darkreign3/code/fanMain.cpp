#ifdef FAN_WIN32
#include <windows.h>
#endif

#include "engine/game/fanLiveppLauncher.hpp"
#include "core/fanPath.hpp"
#include "engine/game/fanLaunchArgs.hpp"
#include "game/fanDarkReign3.hpp"

#ifdef FAN_EDITOR
    #include "editor/fanEditorPlayer.hpp"
#else
    #include "engine/game/fanGamePlayer.hpp"
#endif

//======================================================================================================================================================================================================
//======================================================================================================================================================================================================
int main( int _argc, char* _argv[] )
{
    std::vector<std::string> args;
    args.push_back( _argv[0] );
    // force arguments into the command line
    //args.push_back( "-livepp" );
    //args.push_back( "0" );

    //#ifdef FAN_EDITOR
    //args.push_back( "-autoplay" );
    //args.push_back( "1" );
    //#endif

    args.push_back( "-scene" );
    args.push_back( "scenes/floor.scene" );

    // generates a list of strings from the command line arguments
    for( int i             = 1; i < _argc; i++ ){ args.push_back( _argv[i] ); }

    // Parse the arguments & run the engine
    fan::LaunchArgs     launchArguments;
    fan::LaunchSettings settings;
    launchArguments.Parse( args, settings );

    fan::Path::SetGamePath( GAME_PATH );

    fan::DarkReign3   game;
    const std::string name = "Dark Reign 3";
#ifdef FAN_EDITOR
    settings.mWindowName = "FanEngine Editor - " + name;
    fan::EditorPlayer player( settings, { &game } );
#else
    settings.mWindowName = name;
    fan::GamePlayer player( settings, game );
#endif

#ifdef FAN_LIVEPP
    if( settings.mEnableLivepp )
    {
        fan::LPPLauncher launcher( name, player.mData.mOnLPPSynch );
        player.Run();
    }
    else
#endif
    {
        player.Run();
    }

    return 0;
}

#ifdef FAN_MSVC
//======================================================================================================================================================================================================
//======================================================================================================================================================================================================
int WINAPI WinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd )
{
    (void)_hInstance;
    (void)_hPrevInstance;
    (void)_lpCmdLine;
    (void)_nShowCmd;

    return main( __argc, __argv );
}
#endif