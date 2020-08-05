#pragma once

#include "editor/windows/fanEditorWindow.hpp"
#include "core/fanUnitTest.hpp"

namespace fan
{
    class EcsWorld;
    //========================================================================================================
    // shows the singletons from the game ecs
    //========================================================================================================
    class UnitTestsWindow : public EditorWindow
    {
    public:
        UnitTestsWindow();

    protected:
        struct TestArgument
        {
            using RunMethod = UnitTestResult ( * )();
            const char*      mName;
            RunMethod        mRunMethod;
            UnitTestResult&  mTestDisplay;
        };

        void OnGui( EcsWorld& _world ) override;
        std::vector<TestArgument> GetTests();
        static void DrawTest( const UnitTestResult::TestResult& _testResult );
        static void DrawUnitTest( const TestArgument& _testArgument );
        static void DrawStatusIcon( const UnitTestResult::Status _status );
        static void RunTest( const TestArgument& _testArgument );
        static void ClearTest( const TestArgument& _testArgument );

        UnitTestResult mMeshManagerResult;
        UnitTestResult mMesh2DManagerResult;
        UnitTestResult mTextureManagerResult;
        UnitTestResult mPrefabManagerResult;
        UnitTestResult mFanAssertResult;
        UnitTestResult mButtonResult;
    };
}