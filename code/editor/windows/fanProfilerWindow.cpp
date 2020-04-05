#include "editor/windows/fanProfilerWindow.hpp"

#include "editor/fanModals.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanInput.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	ProfilerWindow::ProfilerWindow() : EditorWindow( "profiler", ImGui::IconType::PROFILER16 )
		, m_color1( 0.49f, 1.f, 0.8f )
		, m_color2( 0.29f, 0.8f, 1.f )
		, m_colorHovered( 0.64f, 0.96f, 0.99f )
	{
		Profiler::Get().onProfilingEnd.Connect( &ProfilerWindow::OnProfilerEnd, this );
		Input::Get().Manager().FindEvent( "freeze_capture" )->Connect( &ProfilerWindow::OnToogleFreezeCapture, this );
	}

	//================================================================================================================================
	// Called when the profiling session ended, copies the intervals before they are cleared & reused by the profiler
	//================================================================================================================================
	void ProfilerWindow::OnProfilerEnd()
	{
		if ( !m_freezeCapture )
		{
			m_intervalsCopy = Profiler::Get().GetIntervals();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProfilerWindow::OnGui()
	{
		SCOPED_PROFILE( profiler )

			// UI
		{

			ImGui::Checkbox( "freeze capture", &m_freezeCapture );
			ImGui::SameLine();	ImGui::FanShowHelpMarker( "press \"END\" to toogle" );
			ImGui::SameLine();	ImGui::DragFloat( "speed", &m_speed, 0.01f, 0.f, 10.f );
		}

		// Returns if no data 
		if ( m_intervalsCopy.size() == 0 )
		{
			return;
		}

		const float width = m_scale * ImGui::GetWindowWidth();

		// Draw graph in a child region
		ImGui::SetNextWindowContentSize( ImVec2( width, 180.f ) );
		ImGui::BeginChild( "##ScrollingRegion", ImVec2( 0.f, 200 ), false, ImGuiWindowFlags_HorizontalScrollbar );
		{

			// Constants		
			const float fontHeight = ImGui::GetFontSize();
			const float fontWidth = 0.75f * fontHeight;
			const float totalTime = TimePoint::SecondsBetween( m_intervalsCopy[ 0 ].time, m_intervalsCopy[ m_intervalsCopy.size() - 1 ].time );
			const TimePoint  beginTime = m_intervalsCopy[ 0 ].time;
			const ImVec2 unsclaledTL = ImGui::GetCursorScreenPos();
			const ImVec2 tl = { unsclaledTL.x, unsclaledTL.y };
			const ImColor colorText = Color::Black.ToImGui();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			const float sizeScrollBar = ( width - ImGui::GetScrollMaxX() ) / width;
			const float posLeftScrollBar = ImGui::GetScrollX() / width;
			const float posCenterScrollBar = posLeftScrollBar + 0.5f * sizeScrollBar;

			const float mouseWindowPos = std::clamp( ( ImGui::GetMousePos().x - ImGui::GetWindowPos().x ) / ImGui::GetWindowSize().x, 0.f, 1.f );
			const float mousePos = posLeftScrollBar + mouseWindowPos * sizeScrollBar;

			if ( ImGui::IsWindowHovered() && ImGui::IsMouseDown( 1 ) )
			{
				// Zoom on mouse scroll
				const  ImGuiIO& io = ImGui::GetIO();
				m_scale += m_speed * io.MouseWheel;
				if ( m_scale < 1.f ) { m_scale = 1.f; }

				// reset scroll to locked position
				const float target = width * ( m_lastScrollPosition - 0.5f * sizeScrollBar );
				ImGui::SetScrollX( target );
			}

			// Set scroll locked position
			if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( 1 ) )
			{
				m_lastScrollPosition = mousePos;
			}

			// Used to represent the last color used at a specific depth ( alternate between two colors )
			std::array< bool, 16 > lastColorStateAtDepth;
			lastColorStateAtDepth.fill( false );

			int endIndex = 0;
			for ( int index = 0; index < m_intervalsCopy.size() / 2; ++index )
			{

				// Find next closed interval
				while ( !m_intervalsCopy[ endIndex ].IsClosing() )
				{
					++endIndex;
				}

				// Find the corresponding opened interval
				const size_t endId = m_intervalsCopy[ endIndex ].id;
				int beginIndex = 0;
				size_t depth = 0; // Stack depth of the scope
				while ( m_intervalsCopy[ beginIndex ].id != endId )
				{
					if ( m_intervalsCopy[ beginIndex ].IsOpening() )
					{
						++depth;
					}
					else 	if ( m_intervalsCopy[ beginIndex ].IsClosing() )
					{
						--depth;
					}
					++beginIndex;
				}
				const Profiler::Interval& begin = m_intervalsCopy[ beginIndex ];
				const Profiler::Interval& end = m_intervalsCopy[ endIndex ];

				// Usefull values
				const float ratio = TimePoint::SecondsBetween( begin.time, end.time ) / totalTime;
				const float offset = TimePoint::SecondsBetween( beginTime, begin.time ) / totalTime * width;
				const float subWidth = ratio * width;
				const ImVec2 tli = { tl.x + offset, tl.y + ( fontHeight + 1 ) * depth };
				const ImVec2 bri = { tli.x + subWidth, tli.y + fontHeight };

				// Alternates colors on elements at the same depth
				assert( depth < lastColorStateAtDepth.size() );
				lastColorStateAtDepth[ depth ] = !lastColorStateAtDepth[ depth ];
				Color color = lastColorStateAtDepth[ depth ] ? m_color1 : m_color2;

				// Draw tooltip			
				if ( ImGui::IsMouseHoveringRect( tli, bri ) )
				{
					color = m_colorHovered;
					ImGui::BeginTooltip();


					std::stringstream ss;
					ss << ( int ) 1000.f * TimePoint::SecondsBetween( begin.time, end.time ) << " ms.";
					ImGui::TextUnformatted( begin.name );
					ImGui::TextUnformatted( ss.str().c_str() );

					ImGui::EndTooltip();
				}

				// Draw rectangle
				draw_list->AddRectFilled( tli, bri, ImGui::GetColorU32( color.ToImGui() ), 0.0f, ImDrawCornerFlags_All );

				// Draw text
				const float textWidth = ImGui::CalcTextSize( begin.name ).x;
				const float iWidth = bri.x - tli.x;
				if ( iWidth > textWidth )
				{
					draw_list->AddText( { tli.x + 0.5f * ( iWidth - textWidth ) , tli.y }, colorText, begin.name, begin.name + std::strlen( begin.name ) );
				}
				++endIndex;
			}
		}
		ImGui::EndChild();
	}
}