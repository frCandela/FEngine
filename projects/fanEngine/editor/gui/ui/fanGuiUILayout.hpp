#pragma once

#include "engine/ui/fanUILayout.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUILayout
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Layout16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUILayout::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName = "Ui layout";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            UILayout& layout = static_cast<UILayout&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
            ImGui::Combo( "type", (int*)&layout.mType, "horizontal\0vertical\0grid\0filled grid\0\0" );

            if( layout.mType == UILayout::FilledGrid )
            {
                ImGui::FanToolTip( "3x3 grid. \n"
                                   "Corners have the size of the top left corner.\n"
                                   "The center is scaled so that the total size of the grid matches the size of the parent transform" );
            }
            else if( layout.mType == UILayout::Grid )
            {
                ImGui::DragInt2( "size", &layout.mGap.x, 1.f, 0, 10 );
            }
            else
            {
                ImGui::DragInt( "gap", &layout.mGap.x, 1.f, -1, 100 );
            }
            ImGui::PopItemWidth();
        }
    };
}