#pragma once

#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include "engine/project/fanLiveppLauncher.hpp"
#include "engine/project/fanLaunchArgs.hpp"
#include "engine/project/fanGameProjectContainer.hpp"
#include "editor/fanEditorProjectContainer.hpp"
#include "game/fanEmptyProject.hpp"

//============================================================================================================
//============================================================================================================
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

    fan::EmptyProject project;
#ifdef FAN_EDITOR
    settings.mWindowName = "empty_project_editor";
    fan::EditorProjectContainer projectContainer( settings, { &project } );
#else
    settings.mWindowName = "empty_project_game";
    fan::GameProjectContainer   projectContainer( settings, project );
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

//============================================================================================================
//============================================================================================================
int WinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd )
{
    (void)_hInstance;
    (void)_hPrevInstance;
    (void)_lpCmdLine;
    (void)_nShowCmd;

    main( __argc, __argv );
}