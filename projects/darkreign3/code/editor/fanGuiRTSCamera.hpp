#pragma once

#include "game/singletons/fanRTSCamera.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiRTSCamera
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Camera16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiRTSCamera::OnGui;
            info.mEditorName = "RTS Camera";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            RTSCamera& rtsCamera = static_cast<RTSCamera&>( _singleton );
            ImGui::DragFixed( "min height", &rtsCamera.mMinHeight );
            ImGui::DragFixed( "max height", &rtsCamera.mMaxHeight );
            ImGui::DragFixed( "zoom speed", &rtsCamera.mZoomSpeed );
            ImGui::DragFixed( "translation speed", &rtsCamera.mTranslationSpeed );
        }
    };
}