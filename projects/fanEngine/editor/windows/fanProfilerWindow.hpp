#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanColor.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Displays profiling data from the  profiler ( SCOPED_PROFILE macros )
    //==================================================================================================================================================================================================
    struct ProfilerWindow : EcsSingleton
    {
    ECS_SINGLETON( ProfilerWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        std::vector<Profiler::Interval> mIntervalsCopy;
        bool                            mFreezeCapture;
        float                           mLastScrollPosition;
        float                           mScale;
        float                           mSpeed;

        void OnProfilerEnd();
        void OnToogleFreezeCapture() { mFreezeCapture = !mFreezeCapture; }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiProfilerWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "profiler";
            info.mIcon       = ImGui::Profiler16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiProfilerWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}