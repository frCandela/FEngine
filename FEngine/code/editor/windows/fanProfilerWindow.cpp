#include "fanGlobalIncludes.h"
#include "editor/windows/fanProfilerWindow.h"

#include "core/scope/fanProfiler.h"
#include "editor/fanModals.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	ProfilerWindow::ProfilerWindow() : EditorWindow("profiler", ImGui::IconType::PROFILER )
	{
		Profiler::Get().onProfilingEnd.Connect( &ProfilerWindow ::OnProfilerEnd, this );
	}

	//================================================================================================================================
	// Called when the profiling session ended, copies the intervals before they are cleared & reused by the profiler
	//================================================================================================================================
	void ProfilerWindow::OnProfilerEnd() {
		if( ! m_freezeCapture ) {
			m_intervalsCopy = Profiler::Get().GetIntervals();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProfilerWindow::OnGui() {
		using Interval = Profiler::Interval;

		static Color colorTest = Color::Yellow;
		static float curr_thickness = 1.f;
		// UI
		{ 
			ImGui::Checkbox("freeze capture", &m_freezeCapture );
			ImGui::ColorEdit3( "color", colorTest.Data(), gui::colorEditFlags );
			ImGui::DragFloat( "curr_thickness", &curr_thickness );
		}

		if ( m_intervalsCopy.size() == 0 ) {
			return;
		}

		// Constants
		const float width = ImGui::GetWindowWidth();
		const float fontHeight = ImGui::GetFontSize();
		const float fontWidth = 0.75f * fontHeight;
		const float totalTime = std::chrono::duration<float> ( m_intervalsCopy[m_intervalsCopy.size() - 1].time - m_intervalsCopy[0].time).count();
		const Interval::TimePoint  beginTime = m_intervalsCopy[0].time;
		const ImVec2 size		  ( width, fontHeight + 2.f );
		const ImVec2 tl = ImGui::GetCursorScreenPos();
		const ImColor colorText = Color::Black.ToImGui();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		int begin = 0;
		int end = 0;

		float tmp = 0;
		while ( begin >= 0 && end < m_intervalsCopy.size() ) {

			// Find next closed interval
			while ( ! m_intervalsCopy[end].IsClosing() ) {
				++ end;
			} 			

			// Find the corresponding opened interval
			const size_t endId = m_intervalsCopy[end].id;
			begin = 0; 
			float height = 0;
			while ( m_intervalsCopy[begin].id != endId ) {
				if ( m_intervalsCopy[begin].IsOpening() ) {
					++height;
				} else 	if ( m_intervalsCopy[begin].IsClosing() ) {
					--height;
				}
				++begin;
			}

			const Interval &  iBegin = m_intervalsCopy[begin];
			const Interval &  iEnd = m_intervalsCopy[end];

			if ( iBegin.id == iEnd.id ) {

				float ratio = std::chrono::duration<float> (  iEnd.time - iBegin.time ).count() / totalTime ;
				float offset = std::chrono::duration<float>( iBegin.time - beginTime).count() / totalTime  * width;
				float subWidth = ratio * width;

				ImVec2 tli = { tl.x + offset, tl.y + (fontHeight+1) * /*height*/  height };
				ImVec2 bri = { tli.x + subWidth, tli.y + fontHeight };


				ImColor color = colorTest.ToImGui();
				if ( ImGui::IsMouseHoveringRect( tli, bri ) ) {
					color = Color::Orange.ToImGui();
					ImGui::BeginTooltip();

					std::chrono::duration<double> delta = iEnd.time - iBegin.time;
					std::stringstream ss ;
					ss << (int)1000.f * delta.count() << " ms.";
					ImGui::TextUnformatted( iBegin.name );
					ImGui::TextUnformatted( ss.str().c_str() );

					ImGui::EndTooltip();
				}

				draw_list->AddRectFilled( tli, bri, color, 0.0f, ImDrawCornerFlags_All );

				const size_t textSize = std::strlen( iBegin.name );
				float iWidth = bri.x - tli.x;
				float textOffset = ( iWidth - fontWidth * textSize ) / 2.f;
				if( iWidth > fontWidth * textSize ) {
					draw_list->AddText( { tli.x + textOffset , tli.y }, colorText, iBegin.name, iBegin.name + textSize );
				}
			}

			++ end;
			++tmp;
		}
	}
}