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

            float speed = editorCamera.mSpeed.ToFloat();
            if( ImGui::DragFloat( "speed", &speed, 1.f, 0.f, 10000.f ) ){ editorCamera.mSpeed = Fixed::FromFloat( speed ); }

            float speedMultiplier = editorCamera.mSpeedMultiplier.ToFloat();
            if( ImGui::DragFloat( "speed multiplier", &speedMultiplier, 1.f, 0.f, 10000.f ) ){ editorCamera.mSpeedMultiplier = Fixed::FromFloat( speed ); }

            ImGui::DragFloat2( "xy sensitivity", &editorCamera.mXYSensitivity[0], 1.f, 0.f, 1.f );
        }
    };
}