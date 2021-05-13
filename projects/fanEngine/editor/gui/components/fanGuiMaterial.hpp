#pragma once

#include "engine/components/fanMaterial.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiMaterial
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Material16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiMaterial::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "material";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Material& material = static_cast<Material&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Filter color
                if( ImGui::Button( "##color" ) ){ material.mColor = Color::sWhite; }
                ImGui::SameLine();
                ImGui::ColorEdit4( "color", material.mColor.Data(), ImGui::fanColorEditFlags );

                if( ImGui::Button( "##shininess" ) ){ material.mShininess = 1; }
                ImGui::SameLine();
                ImGui::DragInt( "shininess", (int*)&material.mShininess, 1, 1, 256 );
                ImGui::SameLine();
                ImGui::FanShowHelpMarker( "sharpness of the specular reflection" );
            }
            ImGui::PopItemWidth();

            ImGui::FanTexturePtr( "mat texture", material.mTexture );
        }
    };
}