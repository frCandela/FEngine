#pragma once

#include "project_spaceships/game/singletons/fanGameCamera.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiGameCamera
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Camera16;
            info.mGroup = EngineGroups::Game;
            info.onGui  = &GuiGameCamera::OnGui;
            info.mEditorName  = "game camera";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            GameCamera& gameCamera = static_cast<GameCamera&>( _component );
            ImGui::DragFloat( "height from target", &gameCamera.mHeightFromTarget, 0.25f, 0.5f, 30.f );
            ImGui::DragFloat2( "margin ratio", &gameCamera.mMarginRatio[0], 0.1f, 0.f, 10.f );
            ImGui::DragFloat( "minSize", &gameCamera.mMinOrthoSize, 0.1f, 0.f, 100.f );
        }
    };
}