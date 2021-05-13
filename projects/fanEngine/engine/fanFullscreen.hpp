#pragma once

#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
    class Window;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct FullScreen
    {
        void SetFullScreen( Window& _window );
        void SetWindowed( Window& _window );

        glm::ivec2 mWindowedSize;
        glm::ivec2 mWindowedPosition;
        bool       mIsFullScreen = false;

        //_window.SetWindowed( mWindowedPosition, mWindowedSize );
        //_window.SetFullscreen();
    };
}