#include "engine/singletons/fanMouse.hpp"
#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"
#include "render/fanWindow.hpp"
#include "editor/fanModals.hpp"

namespace fan
{

    static_assert( Mouse::count == GLFW_MOUSE_BUTTON_LAST + 1 );
    static_assert( Mouse::button1 == GLFW_MOUSE_BUTTON_1 );

    bool Mouse::sLocked = false;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        Mouse& mouse = static_cast<Mouse&>( _singleton );
        mouse.Clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::Clear()
    {
        mLocalPosition  = { 0, 0 };
        mPosition       = { 0, 0 };
        mScrollDelta    = { 0, 0 };
        mPositionDelta  = { 0, 0 };
        mScreenPosition = { 0, 0 };
        mScreenSize     = { 0, 0 };
        mWindowHovered  = false;
        sLocked         = false;
        for( int i = 0; i < Mouse::count; i++ )
        {
            mPressed[i]  = false;
            mReleased[i] = false;
            mDown[i]     = false;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::UpdateData( GLFWwindow* _window )
    {
        Mouse& mouse = Window::GetInputData( _window ).mMouse;
        std::memcpy( this, &mouse, sizeof( Mouse ) );
    }

    //==================================================================================================================================================================================================
    // Coordinate between -1.f and 1.f
    //==================================================================================================================================================================================================
    glm::vec2 Mouse::LocalScreenSpacePosition() const
    {
        if( mScreenSize.x == 0 || mScreenSize.y == 0 ){ return glm::vec2( 0, 0 ); }

        glm::vec2 ratio = 2.f * ( mPosition - mScreenPosition ) / mScreenSize - glm::vec2( 1.f, 1.f );
        ratio.x = std::clamp( ratio.x, -1.f, 1.f );
        ratio.y = std::clamp( ratio.y, -1.f, 1.f );
        return ratio;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::NextFrame( GLFWwindow* _window, const glm::vec2 _viewPosition, const glm::vec2 _viewSize )
    {
        Window::InputData& inputData = Window::GetInputData( _window );
        Mouse            & mouse     = inputData.mMouse;
        mouse.mScreenPosition = _viewPosition;
        mouse.mScreenSize     = _viewSize;
        mouse.mScrollDelta    = glm::vec2( 0.f, 0.f );
        mouse.mPositionDelta  = glm::vec2( 0.f, 0.f );
        for( int i = 0; i < Mouse::count; i++ )
        {
            mouse.mPressed[i]  = false;
            mouse.mReleased[i] = false;
        }
        mouse.mLocalPosition = mouse.mPosition - mouse.mScreenPosition;
        mouse.mWindowHovered = mouse.IsWindowHovered();
        if( mouse.sLocked ){ glfwSetCursorPos( inputData.mWindow, mouse.mPosition.x, mouse.mPosition.y ); }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::SetCallbacks( GLFWwindow* _window )
    {
        glfwSetCursorPosCallback( _window, Mouse::MouseCallback );
        glfwSetMouseButtonCallback( _window, Mouse::MouseButtonCallback );
        glfwSetScrollCallback( _window, Mouse::ScrollCallback );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::MouseCallback( GLFWwindow* _window, double _x, double _y )
    {
        Window::InputData& inputData = Window::GetInputData( _window );
        Mouse            & mouse     = inputData.mMouse;
        mouse.mPositionDelta = glm::vec2( _x, _y ) - mouse.mPosition;
        if( !mouse.sLocked )
        {
            mouse.mPosition      = glm::vec2( (float)_x, (float)_y );
            mouse.mLocalPosition = mouse.mPosition - mouse.mScreenPosition;
            mouse.mWindowHovered = mouse.IsWindowHovered();
        }

        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2( static_cast< float >( _x ), static_cast< float >( _y ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::MouseButtonCallback( GLFWwindow* _window, int _button, int _action, int /*_mods*/ )
    {
        Mouse& mouse = Window::GetInputData( _window ).mMouse;

        fanAssert( _button < Mouse::Button::count );
        ImGuiIO& io = ImGui::GetIO();
        switch( _action )
        {
            case GLFW_PRESS:
                mouse.mPressed[_button] = true;
                mouse.mDown[_button]    = true;
                if( _button < 5 ){ io.MouseDown[_button] = true; }
                break;
            case GLFW_RELEASE:
                mouse.mReleased[_button] = true;
                mouse.mDown[_button]     = false;
                if( _button < 5 ){ io.MouseDown[_button] = false; }
                break;
            case GLFW_REPEAT:

                break;
            default:
                break;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Mouse::ScrollCallback( GLFWwindow* _window, double _xoffset, double _yoffset )
    {
        Mouse& mouse = Window::GetInputData( _window ).mMouse;
        mouse.mScrollDelta += glm::vec2( (float)_xoffset, (float)_yoffset );

        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += (float)_xoffset;
        io.MouseWheel += (float)_yoffset;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Mouse::IsWindowHovered() const
    {
        return mLocalPosition.x >= 0 &&
               mLocalPosition.y >= 0 &&
               mLocalPosition.x < mScreenSize.x &&
               mLocalPosition.y < mScreenSize.y;
    }
}
