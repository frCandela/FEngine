#pragma once

#include "engine/ui/fanUITransform.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiUITransform
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::UiTransform16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUITransform::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName       = "ui transform";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            UITransform& transform = static_cast<UITransform&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                if( ImGui::Button( "##TransUIPos" ) ){ transform.mPosition = glm::vec2( 0, 0 ); }
                ImGui::SameLine();
                ImGui::DragInt2( "position", &transform.mPosition.x, 1, 0 );

                if( ImGui::Button( "##resetUIsize" ) ){ transform.mSize = glm::ivec2( 100, 100 ); }
                ImGui::SameLine();
                ImGui::DragInt2( "size##sizeUI", &transform.mSize.x, 0.1f );
            }
            ImGui::PopItemWidth();
        }
    };
}