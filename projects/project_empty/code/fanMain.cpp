#ifdef FAN_WIN32
#include <windows.h>
#endif

#include "engine/project/fanLiveppLauncher.hpp"
#include "core/fanPath.hpp"
#include "engine/project/fanLaunchArgs.hpp"
#include "game/fanProjectEmpty.hpp"

#ifdef FAN_EDITOR
    #include "editor/fanEditorProjectContainer.hpp"
#else
    #include "engine/project/fanGameProjectContainer.hpp"
#endif

//==============================================================================================================================
//==============================================================================================================================
int main( int _argc, char* _argv[] )
{
    std::vector<std::string> args;
    args.push_back( _argv[0] );
    // force arguments into the command line
// 	args.push_back( "-livepp" );
// 	args.push_back( "0" );
    // generates a list of strings from the command line arguments
    for( int i = 1; i < _argc; i++ ){ args.push_back( _argv[i] ); }

    // Parse the arguments & run the engine
    fan::LaunchArgs     launchArguments;
    fan::LaunchSettings settings;
    launchArguments.Parse( args, settings );

    fan::Path::SetProjectPath( PROJECT_PATH );
    fan::ProjectEmpty project;
#ifdef FAN_EDITOR
    settings.mWindowName = project.mName + "_editor";
    fan::EditorProjectContainer projectContainer( settings, { &project } );
#else
    settings.mWindowName = projectName + "_game";
    fan::GameProjectContainer projectContainer( settings, project );
#endif

#ifdef FAN_LIVEPP
    if( settings.mEnableLivepp )
    {
        fan::LPPLauncher launcher( project.mName );
        launcher.Run( projectContainer );
    }
    else
#endif
    {
        projectContainer.Run();
    }

    return 0;
}

#ifdef FAN_WIN32
//==============================================================================================================================
//==============================================================================================================================
int WinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd )
{
    (void)_hInstance;
    (void)_hPrevInstance;
    (void)_lpCmdLine;
    (void)_nShowCmd;

    main( __argc, __argv );
}
#endif