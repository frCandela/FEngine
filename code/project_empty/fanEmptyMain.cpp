#pragma once

#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include <iostream>
#include "engine/project/fanLaunchSettings.hpp"
#include "project_empty/fanEmptyProject.hpp"
#include "editor/fanEditorHolder.hpp"

//============================================================================================================
//============================================================================================================
int main( int _argc, char* _argv[] )
{
    (void)_argc;
    (void)_argv;

    std::cout << "testoss" << std::endl;

    fan::LaunchSettings settings;
    settings.windowName = "takala";

    fan::EmptyProject project;
    fan::EditorHolder editor( settings, { &project } );
    editor.Run();

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