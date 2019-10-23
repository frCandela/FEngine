#pragma once

#include "editor/windows/fanEditorWindow.h"
#include "core/scope/fanProfiler.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class ProfilerWindow : public EditorWindow {
	public:
		ProfilerWindow();
	protected:
		void OnGui() override;

		void OnProfilerEnd();

	private:
		std::vector<Profiler::Interval> m_intervalsCopy;
	};

}