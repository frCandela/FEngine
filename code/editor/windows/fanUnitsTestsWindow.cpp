#include "editor/windows/fanUnitsTestsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/unit_tests/fanUnitTestFanAssert.hpp"
#include "render/unit_tests/fanUnitTestMeshManager.hpp"
#include "render/unit_tests/fanUnitTestMesh2DManager.hpp"
#include "render/unit_tests/fanUnitTestTextureManager.hpp"
#include "scene/unit_tests/fanUnitTestPrefabManager.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    UnitTestsWindow::UnitTestsWindow() : EditorWindow( "unit tests", ImGui::IconType::NONE ) {}

    //========================================================================================================
    //========================================================================================================
    std::vector<UnitTestsWindow::TestArgument> UnitTestsWindow::GetTests() {
        return {
                { "Mesh manager",       &UnitTestMeshManager::RunTests,     mMeshManagerResult },
                { "Mesh2D manager",     &UnitTestMesh2DManager::RunTests,   mMesh2DManagerResult },
                { "Texture manager",    &UnitTestTextureManager::RunTests,  mTextureManagerResult },
                { "Prefab manager",     &UnitTestPrefabManager::RunTests,   mPrefabManagerResult },
#ifndef NDEBUG
                 { "fanAssert",          &UnitTestFanAssert::RunTests,       mFanAssertResult },
#endif
        };
    }

    //========================================================================================================
    // draw all singletons of the ecs world
    //========================================================================================================
    void UnitTestsWindow::OnGui( EcsWorld& /*_world*/ )
    {
        SCOPED_PROFILE( unit_tests_window );

       const std::vector<TestArgument> tests = GetTests();

        if( ImGui::Button("Test all"))
        {
            for( const TestArgument& testArgument : tests ){ RunTest( testArgument ); }
        }
        ImGui::SameLine();
        if( ImGui::Button("Clear all"))
        {
            for( const TestArgument& testArgument : tests ){ ClearTest( testArgument ); }
        }
#ifndef NDEBUG
        if( System::HasDebugger())
        {
            ImGui::SameLine();
            ImGui::Checkbox( "enable break", &AssertUtils::sFanAssertBreakEnabled );
        }
#endif
        ImGui::Spacing();
        for( const TestArgument& testArgument : tests ){ DrawUnitTest( testArgument ); }
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
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3  );
        DrawStatusIcon( _testArgument.mTestDisplay.mTotalStatus );
        ImGui::SameLine();
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3  );
        if( ImGui::CollapsingHeader( _testArgument.mName ) )
        {
            ImGui::Indent();
            std::string testButtonName = "Test##" + std::string(_testArgument.mName);
            if( ImGui::Button( testButtonName.c_str() ) ){ RunTest( _testArgument ); }
            ImGui::SameLine();
            std::string clearButtonName = "Clear##" + std::string(_testArgument.mName);
            if( ImGui::Button( clearButtonName.c_str() ) ){ ClearTest( _testArgument ); }

            ImGui::Columns( 2 );
            for( const UnitTestResult::TestResult& testResult : _testArgument.mTestDisplay.mTestDisplays )
            {
                DrawTest( testResult );
            }
            ImGui::Columns( 1 );
            ImGui::Unindent();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawTest( const UnitTestResult::TestResult& _testResult )
    {
        ImGui::Text( "%s", _testResult.mName.c_str() );
        ImGui::NextColumn();
        DrawStatusIcon( _testResult.mStatus );
        ImGui::NextColumn();
    }

    //========================================================================================================
    //========================================================================================================
    void UnitTestsWindow::DrawStatusIcon( const UnitTestResult::Status _status )
    {
        const ImVec2 iconSize = { 16, 16 };
        switch( _status )
        {
            case UnitTestResult::Status::Unknown :
                ImGui::Icon(  ImGui::IconType::CHECK_NEUTRAL16, iconSize, ImVec4( 1, 1, 1, 1 ) );
                break;
            case UnitTestResult::Status::Failed :
                ImGui::Icon(  ImGui::IconType::CHECK_FAILED16, iconSize, ImVec4( 1, 0, 0, 1 ) );
                break;
            case UnitTestResult::Status::Success :
                ImGui::Icon(  ImGui::IconType::CHECK_SUCCESS16, iconSize, ImVec4( 0, 1, 0, 1 ) );
                break;
            default:
               fanAssert( false );
                break;
        }
    }
}