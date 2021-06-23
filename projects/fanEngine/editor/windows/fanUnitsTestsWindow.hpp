#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    class EcsWorld;
    //==================================================================================================================================================================================================
    // shows the singletons from the game ecs
    //==================================================================================================================================================================================================
    struct UnitTestsWindow : EcsSingleton
    {
    ECS_SINGLETON( UnitTestsWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        struct TestArgument
        {
            using RunMethod = UnitTestResult ( * )();
            const char* mName;
            RunMethod mRunMethod;
            UnitTestResult& mTestDisplay;
        };

        std::vector<TestArgument> GetTests();
        static void DrawTest( const UnitTestResult::TestResult& _testResult );
        static void DrawUnitTest( const TestArgument& _testArgument );
        static void DrawStatusIcon( const UnitTestResult::Status _status );
        static void RunTest( const TestArgument& _testArgument );
        static void ClearTest( const TestArgument& _testArgument );

        UnitTestResult mFanAssertResult;
        UnitTestResult mGlfwMouseResult;
        UnitTestResult mSignalResult;
        UnitTestResult mEcsResult;
        UnitTestResult mPathResult;
        UnitTestResult mFixedPointResult;
        UnitTestResult mVector3Result;
        UnitTestResult mMatrix3Result;
        UnitTestResult mMatrix4Result;
        UnitTestResult mQuaternionResult;
        UnitTestResult mFxTransformResult;
        UnitTestResult mBitsResult;
        UnitTestResult mQueueResult;
        UnitTestResult mResourcesManagerResult;
        UnitTestResult mStringResult;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUnitTestsWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "unit tests";
            info.mIcon       = ImGui::UnitTests16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiUnitTestsWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}