#pragma once

#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
    class Window;

    //========================================================================================================
    //========================================================================================================
    class FullScreen
    {
    public:
        void OnGui( Window& _window );
        void SavePreviousPositionAndSize( const Window& _window );
        void SetFullScreen( Window& _window );
        void SetWindowed( Window& _window );

    private:
        glm::ivec2 mPrevWindowedSize;
        glm::ivec2 mPrevWindowedPosition;
    };
}