#include "engine/project/fanLiveppLauncher.hpp"
#include "engine/project/fanGameProjectContainer.hpp"
#include "core/fanFileSystem.hpp"
#include "fanLaunchArgsSpaceships.hpp"
#include "game/fanGameServer.hpp"
#include "game/fanGameClient.hpp"

#ifdef FAN_EDITOR
#include "editor/fanEditorProjectContainer.hpp"
#endif

//============================================================================================================
//============================================================================================================
int main( int _argc, char* _argv[] )
{
    std::vector<std::string> args; // command line arguments
    args.push_back( _argv[0] );

// force arguments into the command line
// 	args.push_back( "-livepp" );
// 	args.push_back( "0" );
    args.push_back( "-autoplay" );
    args.push_back( "1" );
    args.push_back( "-scene" );
    args.push_back( "content/scenes/game00.scene" );
    args.push_back( "-server" );
//	args.push_back( "-client" );

    // generates a list of strings from the command line arguments
    for( int i = 1; i < _argc; i++ ){ args.push_back( _argv[i] ); }

    // Parse the arguments & run the engine
    fan::LaunchSettingsSpaceships settings;
    fan::LaunchArgsSpaceships     launchArguments;
    launchArguments.Parse( args, settings );

    fan::FileSystem::SetProjectPath( PROJECT_PATH );

#ifdef FAN_EDITOR
    fan::GameClient             client;
    fan::GameServer             server;
    settings.mWindowName = "spaceships_project_editor_client+server";
    fan::EditorProjectContainer projectContainer( settings, { &client, &server } );
#else
    std::unique_ptr< fan::IProject> project;
    if( settings.mNetworkMode == fan::LaunchSettingsSpaceships::NetworkMode::Server )
    {
        settings.mWindowName = "spaceships_project_game_server";
        project.reset( new fan::GameServer );
    }
    if( settings.mNetworkMode == fan::LaunchSettingsSpaceships::NetworkMode::Client )
    {
        settings.mWindowName = "spaceships_project_game_client";
        project.reset( new fan::GameClient );
    }
    fan::GameProjectContainer   projectContainer( settings, *project );
#endif

    // runs with or without livepp
    if( settings.mEnableLivepp )
    {
        fan::LPPLauncher launcher;
        launcher.Run( projectContainer );
    }
    else
    {
        projectContainer.Run();
    }

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