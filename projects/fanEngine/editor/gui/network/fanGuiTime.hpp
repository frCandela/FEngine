#pragma once

#include "network/singletons/fanTime.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTime
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Time16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiTime::OnGui;
            info.mEditorName = "time";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            Time& time = static_cast<Time&>( _component );
            DrawTimeDeltaSetters( time );
            ImGui::Text( "frame index:   %d", time.mFrameIndex );
        }

        static void DrawTimeDeltaSetters( Time& _time )
        {
            float maxRenderFps = ( 1 / _time.mRenderDelta ).ToFloat();
            if( ImGui::DragFloat( "render frequency", &maxRenderFps, 1.f, 1.f, 3000.f, "%.f" ) )
            {
                _time.mRenderDelta = Fixed::FromFloat( maxRenderFps < 1.f ? 1.f : 1.f / maxRenderFps );
            }

            float maxLogicFps = ( 1 / _time.mLogicDelta ).ToFloat();
            if( ImGui::DragFloat( "logic frequency ", &maxLogicFps, 1.f, 1.f, 3000.f, "%.f" ) )
            {
                _time.mLogicDelta = Fixed::FromFloat( maxLogicFps < 1.f ? 1.f : 1.f / maxLogicFps );
            }
        }
    };
}