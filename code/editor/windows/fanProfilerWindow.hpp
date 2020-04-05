#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
	//================================================================================================================================
	// Displays profiling data from the  profiler ( SCOPED_PROFILE macros )
	//================================================================================================================================
	class ProfilerWindow : public EditorWindow
	{
	public:
		ProfilerWindow();
	protected:
		void OnGui() override;

		void OnProfilerEnd();
	private:
		std::vector<Profiler::Interval> m_intervalsCopy;
		bool	m_freezeCapture = false;
		float	m_lastScrollPosition = 0.f;
		float	m_scale = 1.f;
		float	m_speed = 0.2f;

		const Color m_color1;
		const Color m_color2;
		const Color m_colorHovered;

		void OnToogleFreezeCapture() { m_freezeCapture = !m_freezeCapture; }
	};

}