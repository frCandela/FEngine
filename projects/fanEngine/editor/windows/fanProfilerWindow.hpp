#pragma once

#include "editor/windows/fanEditorWindow.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanColor.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Displays profiling data from the  profiler ( SCOPED_PROFILE macros )
    //==================================================================================================================================================================================================
    class ProfilerWindow : public EditorWindow
    {
    public:
        ProfilerWindow();
    protected:
        void OnGui( EcsWorld& _world ) override;

        void OnProfilerEnd();
    private:
        std::vector<Profiler::Interval> mIntervalsCopy;
        bool                            mFreezeCapture      = false;
        float                           mLastScrollPosition = 0.f;
        float                           mScale              = 1.f;
        float                           mSpeed              = 0.2f;

        const Color mColor1;
        const Color mColor2;
        const Color mColorHovered;

        void OnToogleFreezeCapture() { mFreezeCapture = !mFreezeCapture; }
    };
}