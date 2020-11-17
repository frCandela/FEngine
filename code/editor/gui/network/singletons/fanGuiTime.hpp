#pragma once

#include "network/singletons/fanTime.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiTime
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Time16;
            info.mGroup = EngineGroups::Network;
            info.onGui  = &GuiTime::OnGui;
            info.mEditorName  = "time";
            return info;
        }

        //====================================================================================================
        //====================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            Time& gameTime = static_cast<Time&>( _component );
            ImGui::Text( "frame index:          %d", gameTime.mFrameIndex );
            ImGui::Text( "frame start:          %d", gameTime.mFrameStart );
            ImGui::Text( "logic delta:          %.3f", gameTime.mLogicDelta );
            ImGui::Text( "time scale increment: %.3f", gameTime.mTimeScaleIncrement );
            ImGui::DragFloat( "timescale", &gameTime.mTimeScaleDelta, 0.1f );
        }
    };
}