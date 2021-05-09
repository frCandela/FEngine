#pragma once

#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/ecs/fanEcsSingleton.hpp"

struct GLFWwindow;

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Mouse : public EcsSingleton
    {
    ECS_SINGLETON( Mouse )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        enum CursorState
        {
            disabled = GLFW_CURSOR_DISABLED,
            hidden   = GLFW_CURSOR_HIDDEN,
            normal   = GLFW_CURSOR_NORMAL
        };

        enum Button
        {
            button1, button2, button3, button4, button5, button6, button7, button8, count,
            buttonLeft   = button1,
            buttonRight  = button2,
            buttonMiddle = button3
        };

        glm::vec2                  mLocalPosition;
        glm::vec2                  mPosition;
        glm::vec2                  mScrollDelta;
        glm::vec2                  mPositionDelta;
        glm::vec2                  mScreenPosition;
        glm::vec2                  mScreenSize;
        bool                       mWindowHovered;
        static bool                sLocked;
        static const constexpr int sButtonsCount = Button::count;
        bool                       mPressed[sButtonsCount];
        bool                       mReleased[sButtonsCount];
        bool                       mDown[sButtonsCount];

        void Clear();
        void UpdateData( GLFWwindow* _window );
        glm::vec2 LocalScreenSpacePosition() const;

        static void NextFrame( GLFWwindow* _window, const glm::vec2 _viewPosition, const glm::vec2 _viewSize );
        static void SetCallbacks( GLFWwindow* _window );
        static void MouseCallback( GLFWwindow* _window, double _x, double _y );
        static void MouseButtonCallback( GLFWwindow* _window, int _button, int _action, int _mods );
        static void ScrollCallback( GLFWwindow* _window, double _xoffset, double _yoffset );

    private:
        bool IsWindowHovered() const;
    };
}