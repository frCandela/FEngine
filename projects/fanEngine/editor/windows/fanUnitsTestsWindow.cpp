#include "editor/windows/fanUnitsTestsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/unit_tests/fanUnitTestFanAssert.hpp"
#include "engine/unit_tests/fanUnitTestMouse.hpp"
#include "core/unit_tests/fanUnitTestSignal.hpp"
#include "core/unit_tests/fanUnitTestEcs.hpp"
#include "core/unit_tests/fanUnitTestPath.hpp"
#include "core/unit_tests/fanUnitTestFixedPoint.hpp"
#include "core/unit_tests/fanUnitTestVector3.hpp"
#include "core/unit_tests/fanUnitTestMatrix3.hpp"
#include "core/unit_tests/fanUnitTestMatrix4.hpp"
#include "core/unit_tests/fanUnitTestQuaternion.hpp"
#include "core/unit_tests/fanUnitTestBits.hpp"
#include "core/unit_tests/fanUnitTestResources.hpp"
#include "core/unit_tests/fanUnitTestQueue.hpp"
#include "core/unit_tests/fanUnitTestString.hpp"
#include "engine/unit_tests/fanUnitTestTransform.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::SetInfo( EcsSingletonInfo& /*_info*/ ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        UnitTestsWindow& unitTestsWindow = static_cast<UnitTestsWindow&>( _singleton );
        (void)unitTestsWindow;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<UnitTestsWindow::TestArgument> UnitTestsWindow::GetTests()
    {
        return {
#ifndef NDEBUG
                { "fanAssert", &UnitTestFanAssert::RunTests, mFanAssertResult },
#endif
                { "Mouse", &UnitTestMouse::RunTests, mGlfwMouseResult },
                { "Signal", &UnitTestSignal::RunTests, mSignalResult },
                { "Ecs", &UnitTestEcs::RunTests, mEcsResult },
                { "Path", &UnitTestPath::RunTests, mPathResult },
                { "FixedPoint", &UnitTestFixedPoint::RunTests, mFixedPointResult },
                { "Vector3", &UnitTestVector3::RunTests, mVector3Result },
                { "Matrix3", &UnitTestMatrix3::RunTests, mMatrix3Result },
                { "Matrix4", &UnitTestMatrix4::RunTests, mMatrix4Result },
                { "Quaternion", &UnitTestQuaternion::RunTests, mQuaternionResult },
                { "FxTransform", &UnitTestFxTransform::RunTests, mFxTransformResult },
                { "Bits", &UnitTestBits::RunTests, mBitsResult },
                { "Queue", &UnitTestQueue::RunTests, mQueueResult },
                { "Resource manager", &UnitTestResources::RunTests, mResourcesManagerResult },
                { "String", &UnitTestString::RunTests, mStringResult },
        };
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiUnitTestsWindow::OnGui( EcsWorld&, EcsSingleton& _singleton )
    {
        using TestArgument = UnitTestsWindow::TestArgument;
        UnitTestsWindow& unitTestsWindow = static_cast<UnitTestsWindow&>( _singleton );
        SCOPED_PROFILE( unit_tests_window );

        const std::vector<TestArgument> tests = unitTestsWindow.GetTests();

        ImGui::Icon( ImGui::UnitTests16, { 16, 16 } );
        ImGui::SameLine();
        if( ImGui::Button( "Test all" ) )
        {
            bool oldValueAssertBreakEnabled = AssertUtils::sFanAssertBreakEnabled;

            AssertUtils::sFanAssertBreakEnabled          = false;
            AssertUtils::sFanAssertBreakUnitTestsEnabled = false;
            for( const TestArgument& testArgument : tests ){ UnitTestsWindow::RunTest( testArgument ); }
            AssertUtils::sFanAssertBreakUnitTestsEnabled = true;
            AssertUtils::sFanAssertBreakEnabled          = oldValueAssertBreakEnabled;
        }
        ImGui::SameLine();
        if( ImGui::Button( "Clear all" ) )
        {
            for( const TestArgument& testArgument : tests ){ UnitTestsWindow::ClearTest( testArgument ); }
        }
#ifndef NDEBUG
        if( System::HasDebugger() )
        {
            ImGui::SameLine();
            ImGui::Checkbox( "enable break", &AssertUtils::sFanAssertBreakEnabled );
        }
#endif
        ImGui::Spacing();
        for( const TestArgument& testArgument : tests ){ UnitTestsWindow::DrawUnitTest( testArgument ); }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::RunTest( const TestArgument& _testArgument )
    {
        _testArgument.mTestDisplay = ( *_testArgument.mRunMethod )();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::ClearTest( const TestArgument& _testArgument )
    {
        _testArgument.mTestDisplay = {};
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::DrawUnitTest( const TestArgument& _testArgument )
    {
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3 );
        DrawStatusIcon( _testArgument.mTestDisplay.mTotalStatus );
        ImGui::SameLine();
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3 );
        if( ImGui::CollapsingHeader( _testArgument.mName ) )
        {
            ImGui::Indent();
            std::string testButtonName = "Test##" + std::string( _testArgument.mName );
            if( ImGui::Button( testButtonName.c_str() ) ){ RunTest( _testArgument ); }
            ImGui::SameLine();
            std::string clearButtonName = "Clear##" + std::string( _testArgument.mName );
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::DrawTest( const UnitTestResult::TestResult& _testResult )
    {
        ImGui::Text( "%s", _testResult.mName.c_str() );
        ImGui::NextColumn();
        DrawStatusIcon( _testResult.mStatus );
        ImGui::NextColumn();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void UnitTestsWindow::DrawStatusIcon( const UnitTestResult::Status _status )
    {
        const ImVec2 iconSize = { 16, 16 };
        switch( _status )
        {
            case UnitTestResult::Status::Unknown :
                ImGui::Icon( ImGui::IconType::CheckNeutral16, iconSize, ImVec4( 1, 1, 1, 1 ) );
                break;
            case UnitTestResult::Status::Failed :
                ImGui::Icon( ImGui::IconType::CheckFailed16, iconSize, ImVec4( 1, 0, 0, 1 ) );
                break;
            case UnitTestResult::Status::Success :
                ImGui::Icon( ImGui::IconType::CheckSuccess16, iconSize, ImVec4( 0, 1, 0, 1 ) );
                break;
            default:
                fanAssert( false );
                break;
        }
    }
}