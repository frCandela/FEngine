#include "editor/windows/fanUnitsTestsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    UnitTestsWindow::UnitTestsWindow() : EditorWindow( "unit tests", ImGui::IconType::NONE )
    {}

    //========================================================================================================
    // draw all singletons of the ecs world
    //========================================================================================================
    void UnitTestsWindow::OnGui( EcsWorld& /*_world*/ )
    {
        SCOPED_PROFILE( unit_tests_window );

        if( ImGui::CollapsingHeader("Currency") ) {
            if( ImGui::Button( "Clear" ) )      { mUnitTestsDisplay.clear(); }
            ImGui::SameLine();
            if( ImGui::Button( "Test All " ) )  { mUnitTestsDisplay = UnitTestsCurrency::TestAll(); }


            ImGui::Columns(2);
            ImGui::Separator();
            for( const UnitTestDisplayResult& displayResult : mUnitTestsDisplay )
            {
                ImGui::Text("%s", displayResult.mName.c_str() );
                ImGui::NextColumn();
                ImGui::Icon( displayResult.mIcon, {16,16}, displayResult.mColor );
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
        }
    }
}