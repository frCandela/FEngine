#pragma once

#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include <iostream>
#include "engine/project/fanLaunchSettings.hpp"
#include "engine/project/fanGameProjectContainer.hpp"
#include "editor/fanEditorProjectContainer.hpp"
#include "project_empty/game/fanEmptyProject.hpp"

//============================================================================================================
//============================================================================================================
int main( int _argc, char* _argv[] )
{
    (void)_argc;
    (void)_argv;

    fan::LaunchSettings settings;
    settings.windowName = "takala";

    fan::EmptyProject project;
#ifdef FAN_EDITOR
    fan::EditorProjectContainer editor( settings, { &project } );
    editor.Run();
#else
    fan::GameProjectContainer   game( settings, project );
    game.Run();
#endif



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