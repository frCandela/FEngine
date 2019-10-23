#include "fanGlobalIncludes.h"
#include "editor/windows/fanProfilerWindow.h"

#include "core/scope/fanProfiler.h"

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
		m_intervalsCopy = Profiler::Get().GetIntervals();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProfilerWindow::OnGui() {
		using Interval = Profiler::Interval;

		for (int intervalIndex = 0; intervalIndex < m_intervalsCopy.size(); intervalIndex++){
			const Interval &  interval = m_intervalsCopy[intervalIndex];

			std::stringstream ss;
			ss << interval.name << " " << interval.id << " " << interval.time;

			ImGui::Text( ss.str().c_str() );			
		}
	}
}