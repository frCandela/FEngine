#include "editor/windows/fanProfilerWindow.hpp"

#include <sstream>
#include "editor/fanModals.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	ProfilerWindow::ProfilerWindow() : EditorWindow( "profiler", ImGui::IconType::Profiler16 )
		, mColor1( 0.49f, 1.f, 0.8f )
		, mColor2( 0.29f, 0.8f, 1.f )
		, mColorHovered( 0.64f, 0.96f, 0.99f )
	{
		Profiler::Get().mOnProfilingEnd.Connect( &ProfilerWindow::OnProfilerEnd, this );

        Signal<>& freezeCaptureSignal = * Input::Get().Manager().FindEvent( "freeze_capture" );
        freezeCaptureSignal.Connect( &ProfilerWindow::OnToogleFreezeCapture, this );
	}

	//========================================================================================================
	// Called when the profiling session ended,
	// copies the intervals before they are cleared & reused by the profiler
	//========================================================================================================
	void ProfilerWindow::OnProfilerEnd()
	{
		if ( !mFreezeCapture )
		{
            mIntervalsCopy = Profiler::Get().GetIntervals();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ProfilerWindow::OnGui( EcsWorld& /*_world*/ )
	{
		SCOPED_PROFILE( profiler );

		// UI
		{
			ImGui::Checkbox( "freeze capture", &mFreezeCapture );
			ImGui::SameLine();	ImGui::FanShowHelpMarker( "press \"END\" to toogle" );
			ImGui::SameLine();	ImGui::DragFloat( "speed", &mSpeed, 0.01f, 0.f, 10.f );
		}

		// Returns if no data 
		if ( mIntervalsCopy.size() == 0 )
		{
			return;
		}

		const float width = mScale * ImGui::GetWindowWidth();

		// Draw graph in a child region
		ImGui::SetNextWindowContentSize( ImVec2( width, 180.f ) );
        ImGui::BeginChild( "##ScrollingRegion",
                           ImVec2( 0.f, 200 ),
                           false,
                           ImGuiWindowFlags_HorizontalScrollbar );
        {

			// Constants		
			const float fontHeight = ImGui::GetFontSize();
			const float fontWidth = 0.75f * fontHeight;
            const float totalTime = Clock::SecondsBetween( mIntervalsCopy[0].mTime,
                                                           mIntervalsCopy[mIntervalsCopy.size() - 1].mTime );
			const Clock::TimePoint  beginTime = mIntervalsCopy[ 0 ].mTime;
			const ImVec2 unsclaledTL = ImGui::GetCursorScreenPos();
			const ImVec2 tl = { unsclaledTL.x, unsclaledTL.y };
			const ImColor colorText = Color::sBlack.ToImGui();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			const float sizeScrollBar = ( width - ImGui::GetScrollMaxX() ) / width;
			const float posLeftScrollBar = ImGui::GetScrollX() / width;
			const float posCenterScrollBar = posLeftScrollBar + 0.5f * sizeScrollBar;

            const float mouseWindowPos = std::clamp( ( ImGui::GetMousePos().x - ImGui::GetWindowPos().x ) /
                                                     ImGui::GetWindowSize().x, 0.f, 1.f );
			const float mousePos = posLeftScrollBar + mouseWindowPos * sizeScrollBar;

			if ( ImGui::IsWindowHovered() && ImGui::IsMouseDown( 1 ) )
			{
				// Zoom on mouse scroll
				const  ImGuiIO& io = ImGui::GetIO();
                mScale += mSpeed * io.MouseWheel;
				if ( mScale < 1.f ) { mScale = 1.f; }

				// reset scroll to locked position
				const float target = width * ( mLastScrollPosition - 0.5f * sizeScrollBar );
				ImGui::SetScrollX( target );
			}

			// Set scroll locked position
			if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( 1 ) )
			{
                mLastScrollPosition = mousePos;
			}

			// Used to represent the last color used at a specific depth ( alternate between two colors )
			std::array< bool, 16 > lastColorStateAtDepth;
			lastColorStateAtDepth.fill( false );

			int endIndex = 0;
			for ( int index = 0; index < (int)mIntervalsCopy.size() / 2; ++index )
			{

				// Find next closed interval
				while ( !mIntervalsCopy[ endIndex ].IsClosing() )
				{
					++endIndex;
				}

				// Find the corresponding opened interval
				const size_t endId = mIntervalsCopy[ endIndex ].mID;
				int beginIndex = 0;
				size_t depth = 0; // Stack depth of the scope
				while ( mIntervalsCopy[ beginIndex ].mID != endId )
				{
					if ( mIntervalsCopy[ beginIndex ].IsOpening() )
					{
						++depth;
					}
					else 	if ( mIntervalsCopy[ beginIndex ].IsClosing() )
					{
						--depth;
					}
					++beginIndex;
				}
				const Profiler::Interval& begin = mIntervalsCopy[ beginIndex ];
				const Profiler::Interval& end = mIntervalsCopy[ endIndex ];

				// Useful values
				const float ratio = Clock::SecondsBetween( begin.mTime, end.mTime ) / totalTime;
				const float offset = Clock::SecondsBetween( beginTime, begin.mTime ) / totalTime * width;
				const float subWidth = ratio * width;
				const ImVec2 tli = { tl.x + offset, tl.y + ( fontHeight + 1 ) * depth };
				const ImVec2 bri = { tli.x + subWidth, tli.y + fontHeight };

				// Alternates colors on elements at the same depth
                fanAssert( depth < lastColorStateAtDepth.size() );
				lastColorStateAtDepth[ depth ] = !lastColorStateAtDepth[ depth ];
				Color color = lastColorStateAtDepth[ depth ] ? mColor1 : mColor2;

				// Draw tooltip			
				if ( ImGui::IsMouseHoveringRect( tli, bri ) )
				{
					color = mColorHovered;
					ImGui::BeginTooltip();


					std::stringstream ss;
					ss << ( int ) 1000.f * Clock::SecondsBetween( begin.mTime, end.mTime ) << " ms.";
					ImGui::TextUnformatted( begin.mName );
					ImGui::TextUnformatted( ss.str().c_str() );

					ImGui::EndTooltip();
				}

				// Draw rectangle
                draw_list->AddRectFilled( tli,
                                          bri,
                                          ImGui::GetColorU32( color.ToImGui() ),
                                          0.0f,
                                          ImDrawCornerFlags_All );

				// Draw text
				const float textWidth = ImGui::CalcTextSize( begin.mName ).x;
				const float iWidth = bri.x - tli.x;
				if ( iWidth > textWidth )
				{
                    draw_list->AddText( { tli.x + 0.5f * ( iWidth - textWidth ), tli.y },
                                        colorText,
                                        begin.mName,
                                        begin.mName + std::strlen( begin.mName ) );
				}
				++endIndex;
			}
		}
		ImGui::EndChild();
	}
}