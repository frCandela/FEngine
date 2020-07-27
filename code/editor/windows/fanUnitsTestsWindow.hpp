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
            using RunMethod = UnitTestDisplay ( * )();
            const char*      mName;
            RunMethod        mRunMethod;
            UnitTestDisplay& mTestDisplay;
        };

        void OnGui( EcsWorld& _world ) override;
        std::vector<TestArgument> GetTests();
        static void DrawTest( const UnitTestDisplay::TestDisplay& displayResult );
        static void DrawUnitTest( const TestArgument& _testArgument );
        static void DrawStatusIcon( const UnitTestDisplay::Status _status );
        static void RunTest( const TestArgument& _testArgument );
        static void ClearTest( const TestArgument& _testArgument );

        UnitTestDisplay mCurrencyDisplay;
    };
}