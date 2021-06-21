#pragma once

#include "engine/ui/fanUIProgressBar.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/gui/fanGuiComponentPtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUIProgressBar
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::UiProgressBar16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIProgressBar::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName = "Ui progress bar";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            UIProgressBar& progressBar = static_cast<UIProgressBar&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                bool update = false;
                update |= ImGui::DragFloat( "progress", &progressBar.mProgress, 0.01f, 0.f, 1.f );
                update |= ImGui::DragInt( "max width", &progressBar.mMaxSize, 0.01f );
                update |= ImGui::FanComponent( "target transform", progressBar.mTargetTransform );

                if( update ){ progressBar.SetProgress( progressBar.mProgress ); }
            }
            ImGui::PopItemWidth();
        }
    };
}