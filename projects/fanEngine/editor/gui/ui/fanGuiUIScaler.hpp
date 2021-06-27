#pragma once

#include "engine/ui/fanUIScaler.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUIScaler
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::UiTransform16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIScaler::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName = "Ui scaler";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            UIScaler& scaler = static_cast<UIScaler&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
            ImGui::Combo( "scale direction", (int*)&scaler.mScaleDirection, "Horizontal\0Vertical\0HorizontalVertical\0\0" );
            ImGui::FanToolTip( "Copies the scale of its parent" );
            ImGui::PopItemWidth();
        }
    };
}