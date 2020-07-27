#include "editor/windows/fanUnitsTestsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "editor/unit_tests/fanUnitTestCurrency.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    UnitTestsWindow::UnitTestsWindow() : EditorWindow( "unit tests", ImGui::IconType::NONE ) {}

    //========================================================================================================
    // draw all singletons of the ecs world
    //========================================================================================================
    void UnitTestsWindow::OnGui( EcsWorld& /*_world*/ )
    {
        SCOPED_PROFILE( unit_tests_window );

        std::vector<TestArgument> tests = GetTests();

        if( ImGui::Button("Test all"))
        {
            for( TestArgument& testArgument : tests ){ RunTest( testArgument ); }
        }
        ImGui::SameLine();
        if( ImGui::Button("Clear all"))
        {
            for( TestArgument& testArgument : tests ){ ClearTest( testArgument ); }
        }
        ImGui::SameLine();
        ImGui::Checkbox("enable break", &UnitTestsUtils::sBreakWhenUnitTestFails );
        ImGui::Spacing();
        for( const TestArgument& testArgument : tests )
        {
            DrawUnitTest( testArgument );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::RunTest( const TestArgument& _testArgument )
    {
        _testArgument.mTestDisplay =  ( *_testArgument.mRunMethod )();
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::ClearTest( const TestArgument& _testArgument )
    {
        _testArgument.mTestDisplay = {};
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawUnitTest( const TestArgument& _testArgument )
    {
        DrawStatusIcon( _testArgument.mTestDisplay.mTotalStatus );
        ImGui::SameLine();
        if( ImGui::CollapsingHeader( _testArgument.mName ) )
        {
            ImGui::Indent();
            if( ImGui::Button( "Test" ) ){ RunTest( _testArgument ); }
            ImGui::SameLine();
            if( ImGui::Button( "Clear" ) ){ ClearTest( _testArgument ); }

            ImGui::Columns( 2 );
            for( const UnitTestDisplay::TestDisplay& testDisplay : _testArgument.mTestDisplay.mTestDisplays )
            {
                DrawTest( testDisplay );
            }
            ImGui::Columns( 1 );
            ImGui::Unindent();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawTest( const UnitTestDisplay::TestDisplay& _testDisplay )
    {
        ImGui::Text( "%s", _testDisplay.mName.c_str() );
        ImGui::NextColumn();
        DrawStatusIcon( _testDisplay.mStatus );
        ImGui::NextColumn();
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawStatusIcon( const UnitTestDisplay::Status _status )
    {
        switch( _status )
        {
            case UnitTestDisplay::Status::Unknown :
                ImGui::Icon(  ImGui::IconType::CHECK_NEUTRAL16, { 16, 16 }, ImVec4( 1, 1, 1, 1 ) );
                break;
            case UnitTestDisplay::Status::Failed :
                ImGui::Icon(  ImGui::IconType::CHECK_FAILED16, { 16, 16 }, ImVec4( 1, 0, 0, 1 ) );
                break;
            case UnitTestDisplay::Status::Success :
                ImGui::Icon(  ImGui::IconType::CHECK_SUCCESS16, { 16, 16 }, ImVec4( 0, 1, 0, 1 ) );
                break;
            default:
                assert( false );
                break;
        }
    }

    //========================================================================================================
    //========================================================================================================
    std::vector<UnitTestsWindow::TestArgument> UnitTestsWindow::GetTests() {
        return {
                { "Currency", &UnitTestCurrency::RunTests, mCurrencyDisplay }
        };
    }
}