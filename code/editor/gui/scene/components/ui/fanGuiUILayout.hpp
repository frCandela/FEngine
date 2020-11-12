#pragma once

#include "scene/components/ui/fanUILayout.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiUILayout
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Layout16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUILayout::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName       = "ui layout";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            UILayout& layout = static_cast<UILayout&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
            ImGui::Combo( "type", (int*)&layout.mType, "horizontal\0vertical\0\0" );
            ImGui::DragInt( "gap", &layout.mGap, 1.f, -1, 100 );
            ImGui::PopItemWidth();
        }
    };
}