#pragma once

#include "game/components/fanAnimScale.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiAnimScale
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Transform16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiAnimScale::OnGui;
            info.mEditorName = "anim scale";
            info.mEditorPath = "game/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            AnimScale& animScale = static_cast<AnimScale&>( _component );
            ImGui::DragFixed3( "source scale", &animScale.mSourceScale.x );
            ImGui::DragFixed3( "target scale", &animScale.mTargetScale.x );
            ImGui::DragFixed( "duration", &animScale.mDuration );
        }
    };
}