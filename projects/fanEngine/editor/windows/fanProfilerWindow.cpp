#include "editor/windows/fanProfilerWindow.hpp"

#include <sstream>
#include "platform/input/fanInputManager.hpp"
#include "platform/input/fanInput.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    static const Color sColor1       = Color( 0.49f, 1.f, 0.8f );
    static const Color sColor2       = Color( 0.29f, 0.8f, 1.f );
    static const Color sColorHovered = Color( 0.64f, 0.96f, 0.99f );

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProfilerWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProfilerWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        ProfilerWindow& profilerWindow = static_cast<ProfilerWindow&>( _singleton );
        profilerWindow.mIntervalsCopy.clear();
        profilerWindow.mFreezeCapture      = false;
        profilerWindow.mLastScrollPosition = 0.f;
        profilerWindow.mScale              = 1.f;
        profilerWindow.mSpeed              = 0.2f;

        Profiler::Get().mOnProfilingEnd.Connect( &ProfilerWindow::OnProfilerEnd, &profilerWindow );
        Signal<>& freezeCaptureSignal = *Input::Get().Manager().FindEvent( "freeze_capture" );
        freezeCaptureSignal.Connect( &ProfilerWindow::OnToogleFreezeCapture, &profilerWindow );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiProfilerWindow::OnGui( EcsWorld&, EcsSingleton& _singleton )
    {
        ProfilerWindow& profilerWindow = static_cast<ProfilerWindow&>( _singleton );
        (void)profilerWindow;

        SCOPED_PROFILE( profiler );

        // UI
        {
            ImGui::Checkbox( "freeze capture", &profilerWindow.mFreezeCapture );
            ImGui::SameLine();
            ImGui::FanShowHelpMarker( "press \"END\" to toogle" );
            ImGui::SameLine();
            ImGui::DragFloat( "speed", &profilerWindow.mSpeed, 0.01f, 0.f, 10.f );
        }

        // Returns if no data
        if( profilerWindow.mIntervalsCopy.size() == 0 )
        {
            return;
        }

        const float width = profilerWindow.mScale * ImGui::GetWindowWidth();

        // Draw graph in a child region
        ImGui::SetNextWindowContentSize( ImVec2( width, 180.f ) );
        ImGui::BeginChild( "##ScrollingRegion", ImVec2( 0.f, 200 ), false, ImGuiWindowFlags_HorizontalScrollbar );
        {

            // Constants
            const float            fontHeight  = ImGui::GetFontSize();
            const float            totalTime   = Clock::SecondsBetween( profilerWindow.mIntervalsCopy[0].mTime,
                                                                        profilerWindow.mIntervalsCopy[profilerWindow.mIntervalsCopy.size() - 1].mTime );
            const Clock::TimePoint beginTime   = profilerWindow.mIntervalsCopy[0].mTime;
            const ImVec2           unsclaledTL = ImGui::GetCursorScreenPos();
            const ImVec2           tl          = { unsclaledTL.x, unsclaledTL.y };
            const ImColor          colorText   = Color::sBlack.ToImGui();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            const float sizeScrollBar    = ( width - ImGui::GetScrollMaxX() ) / width;
            const float posLeftScrollBar = ImGui::GetScrollX() / width;

            const float mouseWindowPos = std::clamp( ( ImGui::GetMousePos().x - ImGui::GetWindowPos().x ) /
                                                     ImGui::GetWindowSize().x, 0.f, 1.f );
            const float mousePos       = posLeftScrollBar + mouseWindowPos * sizeScrollBar;

            if( ImGui::IsWindowHovered() && ImGui::IsMouseDown( 1 ) )
            {
                // Zoom on mouse scroll
                const ImGuiIO& io = ImGui::GetIO();
                profilerWindow.mScale += profilerWindow.mSpeed * io.MouseWheel;
                if( profilerWindow.mScale < 1.f ){ profilerWindow.mScale = 1.f; }

                // reset scroll to locked position
                const float target = width * ( profilerWindow.mLastScrollPosition - 0.5f * sizeScrollBar );
                ImGui::SetScrollX( target );
            }

            // Set scroll locked position
            if( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( 1 ) )
            {
                profilerWindow.mLastScrollPosition = mousePos;
            }

            // Used to represent the last color used at a specific depth ( alternate between two colors )
            std::array<bool, 16> lastColorStateAtDepth;
            lastColorStateAtDepth.fill( false );

            int      endIndex = 0;
            for( int index    = 0; index < (int)profilerWindow.mIntervalsCopy.size() / 2; ++index )
            {

                // Find next closed interval
                while( !profilerWindow.mIntervalsCopy[endIndex].IsClosing() )
                {
                    ++endIndex;
                }

                // Find the corresponding opened interval
                const size_t endId      = profilerWindow.mIntervalsCopy[endIndex].mID;
                int          beginIndex = 0;
                size_t       depth      = 0; // Stack depth of the scope
                while( profilerWindow.mIntervalsCopy[beginIndex].mID != endId )
                {
                    if( profilerWindow.mIntervalsCopy[beginIndex].IsOpening() )
                    {
                        ++depth;
                    }
                    else if( profilerWindow.mIntervalsCopy[beginIndex].IsClosing() )
                    {
                        --depth;
                    }
                    ++beginIndex;
                }
                const Profiler::Interval& begin = profilerWindow.mIntervalsCopy[beginIndex];
                const Profiler::Interval& end   = profilerWindow.mIntervalsCopy[endIndex];

                // Useful values
                const float  ratio    = Clock::SecondsBetween( begin.mTime, end.mTime ) / totalTime;
                const float  offset   = Clock::SecondsBetween( beginTime, begin.mTime ) / totalTime * width;
                const float  subWidth = ratio * width;
                const ImVec2 tli      = { tl.x + offset, tl.y + ( fontHeight + 1 ) * depth };
                const ImVec2 bri      = { tli.x + subWidth, tli.y + fontHeight };

                // Alternates colors on elements at the same depth
                fanAssert( depth < lastColorStateAtDepth.size() );
                lastColorStateAtDepth[depth] = !lastColorStateAtDepth[depth];
                Color color = lastColorStateAtDepth[depth] ? sColor1 : sColor2;

                // Draw tooltip
                if( ImGui::IsMouseHoveringRect( tli, bri ) )
                {
                    color = sColorHovered;
                    ImGui::BeginTooltip();

                    std::stringstream ss;
                    ss << (int)1000.f * Clock::SecondsBetween( begin.mTime, end.mTime ) << " ms.";
                    ImGui::TextUnformatted( begin.mName );
                    ImGui::TextUnformatted( ss.str().c_str() );

                    ImGui::EndTooltip();
                }

                // Draw rectangle
                draw_list->AddRectFilled( tli, bri, ImGui::GetColorU32( color.ToImGui() ), 0.0f, ImDrawCornerFlags_All );

                // Draw text
                const float textWidth = ImGui::CalcTextSize( begin.mName ).x;
                const float iWidth    = bri.x - tli.x;
                if( iWidth > textWidth )
                {
                    draw_list->AddText( { tli.x + 0.5f * ( iWidth - textWidth ), tli.y }, colorText, begin.mName, begin.mName + std::strlen( begin.mName ) );
                }
                ++endIndex;
            }
        }
        ImGui::EndChild();
    }

    //==================================================================================================================================================================================================
    // Called when the profiling session ended,
    // copies the intervals before they are cleared & reused by the profiler
    //==================================================================================================================================================================================================
    void ProfilerWindow::OnProfilerEnd()
    {
        if( !mFreezeCapture )
        {
            mIntervalsCopy = Profiler::Get().GetIntervals();
        }
    }
}