#include "scene/singletons/fanInputMouse.hpp"
#include "core/fanAssert.hpp"
#include "render/fanWindow.hpp"
#include "editor/fanModals.hpp"

namespace fan{
    static_assert( Mouse2::count == GLFW_MOUSE_BUTTON_LAST + 1 );
    static_assert( Mouse2::button1 == GLFW_MOUSE_BUTTON_1 );

    //========================================================================================================
    //========================================================================================================
    void Mouse2::SetInfo( EcsSingletonInfo& _info )
    {
        _info.icon  = ImGui::IconType::INPUT16;
        _info.group = EngineGroups::Scene;
        _info.name  = "input mouse";
        _info.onGui = &Mouse2::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void Mouse2::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        Mouse2& mouse = static_cast<Mouse2&>( _singleton );
        mouse.Clear();
    }

    //========================================================================================================
    //========================================================================================================
    void Mouse2::Clear( )
    {
        mScreenPosition = {0,0};
        mPosition = {0,0};
        mLocalPosition = {0,0};
        mPositionDelta = {0,0};
        mScrollDelta = {0,0};
        mLocked = false;
        for( int i=0 ; i < Mouse2::count; i++ )
        {
            mPressed[i] = false;
            mReleased[i] = false;
            mDown[i] = false;
        }
    }

    //================================================================================================================================
    // Coordinate between -1.f and 1.f
    //================================================================================================================================
    glm::vec2 Mouse2::LocalScreenSpacePosition() const
    {
        if( mScreenSize.x == 0 || mScreenSize.y == 0 ) { return  glm::vec2(0,0); }

        glm::vec2 ratio = 2.f * ( mPosition - mScreenPosition ) / mScreenSize - glm::vec2 ( 1.f, 1.f );
        ratio.x = std::clamp( ratio.x, -1.f, 1.f );
        ratio.y = std::clamp( ratio.y, -1.f, 1.f );
        return ratio;
    }

    //========================================================================================================
    //========================================================================================================
    void Mouse2::NextFrame( GLFWwindow* _window, const glm::vec2 _position, const glm::vec2 _size  )
    {
        Mouse2& mouse = Window::GetInputData( _window ).mMouse;
        mouse.mScreenPosition = _position;
        mouse.mScreenSize = _size;
        mouse.mScrollDelta = glm::vec2( 0.f, 0.f );
        mouse.mPositionDelta = glm::vec2( 0.f, 0.f );
        for( int i=0 ; i < Mouse2::count; i++ )
        {
            mouse.mPressed[i] = false;
            mouse.mReleased[i] = false;
        }
       mouse.mLocalPosition =  mouse.mPosition - mouse.mScreenPosition;
    }

    //========================================================================================================
    //========================================================================================================
    void Mouse2::MouseCallback( GLFWwindow* _window, double _x, double _y )
    {
        Mouse2& mouse = Window::GetInputData( _window ).mMouse;
        mouse.mPositionDelta = glm::vec2( _x, _y ) - mouse.mPosition;
        if( !mouse.mLocked )
        {
            mouse.mPosition = glm::vec2( (float)_x, (float)_y );
            mouse.mLocalPosition =  mouse.mPosition - mouse.mScreenPosition;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void Mouse2::MouseButtonCallback( GLFWwindow* _window, int _button, int _action, int /*_mods*/ )
    {
        Mouse2& mouse = Window::GetInputData( _window ).mMouse;

        fanAssert( _button < Mouse2::Button::count );
        switch(  _action )
        {
            case GLFW_PRESS:
                mouse.mPressed[ _button ] = true;
                mouse.mDown[ _button ] = true;
                break;
            case GLFW_RELEASE:
                mouse.mReleased[ _button ] = true;
                mouse.mDown[ _button ] = false;
                break;
            case GLFW_REPEAT:

                break;
            default:
                break;
        }
    }


    //========================================================================================================
    //========================================================================================================
    void Mouse2::ScrollCallback( GLFWwindow* _window, double _xoffset, double _yoffset )
    {
        Mouse2& mouse = Window::GetInputData( _window ).mMouse;
        mouse.mScrollDelta += glm::vec2 ( (float)_xoffset, (float)_yoffset );
    }

    //========================================================================================================
    //========================================================================================================
    void Mouse2::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        ImGui::Indent();
        ImGui::Indent();
        {
            Mouse2& inputMouse = static_cast<Mouse2&>( _singleton );
            ImGui::PushReadOnly();
            ImGui::InputFloat2( "position", &inputMouse.mPosition.x );
            ImGui::PopReadOnly();
        }
        ImGui::Unindent();
        ImGui::Unindent();
    }
}
