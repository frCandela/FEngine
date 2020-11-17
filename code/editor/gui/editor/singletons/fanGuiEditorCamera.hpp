#pragma once

#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiEditorCamera
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName  = "editor camera";
            info.mIcon  = ImGui::Camera16;
            info.mGroup = EngineGroups::Editor;
            info.onGui  = &GuiEditorCamera::OnGui;
            return info;
        }

        //====================================================================================================
        //====================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );
            ImGui::DragFloat( "speed", &editorCamera.mSpeed, 1.f, 0.f, 10000.f );
            ImGui::DragFloat( "speed multiplier", &editorCamera.mSpeedMultiplier, 1.f, 0.f, 10000.f );
            ImGui::DragFloat2( "xy sensitivity", &editorCamera.mXYSensitivity[0], 1.f, 0.f, 1.f );
        }
    };
}