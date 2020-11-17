#include "engine/fanFullscreen.hpp"
#include "render/fanWindow.hpp"
#include "core/memory/fanSerializedValues.hpp"
#include "imgui/imgui.h"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FullScreen::SavePreviousPositionAndSize( const Window& _window )
    {
        mPrevWindowedPosition = _window.GetPosition();
        mPrevWindowedSize = _window.GetSize();
    }

    //========================================================================================================
    //========================================================================================================
    void FullScreen::SetFullScreen( Window& _window )
    {
        SavePreviousPositionAndSize( _window );
        _window.SetFullscreen();
    }

    //========================================================================================================
    //========================================================================================================
    void FullScreen::SetWindowed( Window& _window )
    {
        _window.SetWindowed( mPrevWindowedPosition,
                             mPrevWindowedSize );
    }

    //========================================================================================================
    //========================================================================================================
    void FullScreen::OnGui( Window& _window )
    {
        bool isFullscreen = _window.IsFullscreen();
        if( ImGui::Checkbox( "fullscreen", &isFullscreen ) )
        {
            if( isFullscreen )
            {
                SetFullScreen( _window );
            }
            else
            {
                SetWindowed( _window );
            }
        }
    }
}
