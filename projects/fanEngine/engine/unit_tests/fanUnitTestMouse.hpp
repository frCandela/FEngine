#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "render/fanWindow.hpp"
#include "engine/singletons/fanMouse.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestMouse : public UnitTest<UnitTestMouse>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestMouse::TestButtonPressed,            "Button pressed" },
                     { &UnitTestMouse::TestButtonReleased,           "Button released" },
                     { &UnitTestMouse::TestButtonDown,               "Button down" },
                     { &UnitTestMouse::TestScroll,                   "Scroll delta" },
                     { &UnitTestMouse::TestPosition,                 "Position" },
                     { &UnitTestMouse::TestLocalPosition,            "Local position" },
                     { &UnitTestMouse::TestLockCursor,               "Lock cursor" },
                     { &UnitTestMouse::TestPositionDelta,            "Position delta" },
                     { &UnitTestMouse::TestPositionLocalScreenSpace, "Position local screen space" },
                     { &UnitTestMouse::TestWIndowHovered,            "Window hovered" },
            };
        }

        void Create() override
        {
            // we emulate a glfw window by redirecting the WindowUserPointer to our test data
            mOldFunc                  = Window::sGetWindowUserPtr;
            Window::sGetWindowUserPtr = &UnitTestGetWindowUserPointer;
            mInputData.mMouse.Clear();
        }

        void Destroy() override
        {
            Window::sGetWindowUserPtr = mOldFunc;
        }

        // there is no window, only input data undercover :p
        GLFWwindow* GetWindow() { return (GLFWwindow*)( &mInputData ); }
        static void* UnitTestGetWindowUserPointer( GLFWwindow* _window ) { return _window; }

        Window::GetWindowUserPtrFunc mOldFunc = nullptr;
        Window::InputData            mInputData;

        void TestButtonPressed()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( !mouse.mPressed[Mouse::button1] );
            Mouse::MouseButtonCallback( window, 0, GLFW_PRESS, -1 );
            TEST_ASSERT( mouse.mPressed[Mouse::button1] );
            Mouse::NextFrame( window, glm::vec2( 0.f, 0.f ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( !mouse.mPressed[Mouse::button1] );
        }

        void TestButtonReleased()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( !mouse.mReleased[Mouse::button1] );
            Mouse::MouseButtonCallback( window, 0, GLFW_RELEASE, -1 );
            TEST_ASSERT( mouse.mReleased[Mouse::button1] );
            Mouse::NextFrame( window, glm::vec2( 0.f, 0.f ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( !mouse.mReleased[Mouse::button1] );
        }

        void TestButtonDown()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( !mouse.mDown[Mouse::button1] );
            Mouse::MouseButtonCallback( window, 0, GLFW_PRESS, -1 );
            TEST_ASSERT( mouse.mDown[Mouse::button1] );
            Mouse::NextFrame( window, glm::vec2( 0.f, 0.f ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mDown[Mouse::button1] );
            Mouse::MouseButtonCallback( window, 0, GLFW_RELEASE, -1 );
            TEST_ASSERT( !mouse.mDown[Mouse::button1] );
        }

        void TestScroll()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( mouse.mScrollDelta == glm::vec2( 0.f, 0.f ) );
            Mouse::ScrollCallback( window, 1., 2. );
            TEST_ASSERT( mouse.mScrollDelta == glm::vec2( 1.f, 2.f ) );
            Mouse::ScrollCallback( window, 2., 3. );
            TEST_ASSERT( mouse.mScrollDelta == glm::vec2( 3.f, 5.f ) );
            Mouse::NextFrame( window, glm::vec2( 0.f, 0.f ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mScrollDelta == glm::vec2( 0.f, 0.f ) );
        }

        void TestPosition()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            // full window position
            TEST_ASSERT( mouse.mPosition == glm::vec2( 0.f, 0.f ) );
            Mouse::MouseCallback( window, 200., 300. );
            TEST_ASSERT( mouse.mPosition == glm::vec2( 200.f, 300.f ) );
            Mouse::NextFrame( window, glm::vec2( 0.f, 0.f ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mPosition == glm::vec2( 200.f, 300.f ) );
            Mouse::MouseCallback( window, 600., 100. );
            TEST_ASSERT( mouse.mPosition == glm::vec2( 600.f, 100.f ) );
        }

        void TestLocalPosition()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( mouse.mScreenPosition == glm::vec2( 0, 0 ) );
            TEST_ASSERT( mouse.mLocalPosition == glm::vec2( 0, 0 ) );
            const glm::vec2 screenpos = glm::vec2( 100.f, 200.f );
            Mouse::NextFrame( window, screenpos, glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mScreenPosition == screenpos );
            TEST_ASSERT( mouse.mLocalPosition == ( mouse.mPosition - screenpos ) );
            Mouse::MouseCallback( window, 500., 200. );
            TEST_ASSERT( mouse.mLocalPosition == ( mouse.mPosition - screenpos ) );
        }

        void TestLockCursor()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( !mouse.sLocked );
            Mouse::MouseCallback( window, 100., 200. );
            TEST_ASSERT( mouse.mPosition == glm::vec2( 100.f, 200.f ) );
            mouse.sLocked = true;
            Mouse::MouseCallback( window, 200., 300. );
            TEST_ASSERT( mouse.mPosition == glm::vec2( 100.f, 200.f ) );
            TEST_ASSERT( mouse.mPositionDelta == glm::vec2( 100.f, 100.f ) );
        }

        void TestPositionDelta()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( mouse.mPositionDelta == glm::vec2( 0.f, 0.f ) );
            Mouse::MouseCallback( window, 10., 20. );
            TEST_ASSERT( mouse.mPositionDelta == glm::vec2( 10.f, 20.f ) );
            Mouse::NextFrame( window, glm::vec2( 0, 0 ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mPositionDelta == glm::vec2( 0.f, 0.f ) );
        }

        void TestPositionLocalScreenSpace()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();

            TEST_ASSERT( mouse.mScreenPosition == glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mScreenSize == glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.mLocalPosition == glm::vec2( 0.f, 0.f ) );
            const glm::vec2 winPos  = glm::vec2( 10, 20 );
            const glm::vec2 winSize = glm::vec2( 30, 40 );
            Mouse::NextFrame( window, winPos, winSize );
            Mouse::MouseCallback( window, winPos.x + winSize.x / 2.f, winPos.y + winSize.y / 2.f );
            TEST_ASSERT( mouse.LocalScreenSpacePosition() == glm::vec2( 0.f, 0.f ) );
            Mouse::MouseCallback( window, winPos.x, winPos.y );
            TEST_ASSERT( mouse.LocalScreenSpacePosition() == glm::vec2( -1.f, -1.f ) );
            Mouse::MouseCallback( window, winPos.x + winSize.x, winPos.y + winSize.y );
            TEST_ASSERT( mouse.LocalScreenSpacePosition() == glm::vec2( 1.f, 1.f ) );
            Mouse::MouseCallback( window, -1000., 1000. );
            TEST_ASSERT( mouse.LocalScreenSpacePosition() == glm::vec2( -1.f, 1.f ) );
            Mouse::MouseCallback( window, 1000., -1000. );
            TEST_ASSERT( mouse.LocalScreenSpacePosition() == glm::vec2( 1.f, -1.f ) );
            Mouse::NextFrame( window, glm::vec2( 0.f, 0.f ), glm::vec2( 0.f, 0.f ) );
            TEST_ASSERT( mouse.LocalScreenSpacePosition() == glm::vec2( 0.f, 0.f ) );
        }

        void TestWIndowHovered()
        {
            Mouse     & mouse  = mInputData.mMouse;
            GLFWwindow* window = GetWindow();
            TEST_ASSERT( mouse.mWindowHovered == false );
            const glm::vec2 winPos  = glm::vec2( 10, 20 );
            const glm::vec2 winSize = glm::vec2( 30, 40 );
            Mouse::NextFrame( window, winPos, winSize );
            Mouse::MouseCallback( window, winPos.x, winPos.y );
            TEST_ASSERT( mouse.mWindowHovered == true );
            Mouse::MouseCallback( window, winPos.x + winSize.x, winPos.y + winSize.y );
            TEST_ASSERT( mouse.mWindowHovered == false );
            Mouse::MouseCallback( window, winPos.x + winSize.x - 1, winPos.y + winSize.y - 1 );
            TEST_ASSERT( mouse.mWindowHovered == true );
            Mouse::NextFrame( window, glm::vec2( 0, 0 ), winSize );
            TEST_ASSERT( mouse.mWindowHovered == false );
        }
    };
}