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
            info.mEditorName = "Material";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Material& material = static_cast<Material&>( _component );

            if( ImGui::ButtonIcon(ImGui::IconType::Plus8, {8,8}) &&  material.mMaterials.size() < Material::sMaxSubMaterials)
            {
                material.mMaterials.push_back({});
            }
            ImGui::SameLine();
            if( ImGui::ButtonIcon(ImGui::IconType::Minus8, {8,8}) &&  material.mMaterials.size() > 1)
            {
                material.mMaterials.pop_back();
            }
            for( int i = 0; i < (int)material.mMaterials.size(); ++i )
            {
                SubMaterial& subMaterial = material.mMaterials[i];
                if( ImGui::CollapsingHeader(("material " + std::to_string(i)).c_str()) )
                {
                    ImGui::PushID(i);
                    ImGui::Indent();
                    ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 32 );
                    {
                        // Filter color
                        if( ImGui::Button( "##color" ) ){ subMaterial.mColor = Color::sWhite; }
                        ImGui::SameLine();
                        ImGui::ColorEdit4( "color", subMaterial.mColor.Data(), ImGui::fanColorEditFlags );

                        if( ImGui::Button( "##shininess" ) ){ subMaterial.mShininess = 1; }
                        ImGui::SameLine();
                        ImGui::DragInt( "shininess", (int*)&subMaterial.mShininess, 1, 1, 256 );
                        ImGui::SameLine();
                        ImGui::FanShowHelpMarker( "sharpness of the specular reflection" );
                    }
                    ImGui::PopItemWidth();
                    ImGui::FanResourcePtr<Texture>( "mat texture", subMaterial.mTexture );
                    ImGui::Unindent();
                    ImGui::PopID();
                }
            }
        }
    };
}