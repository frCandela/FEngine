#pragma once

#include "engine/ui/fanUIRenderer.hpp"
#include "engine/fanSceneTags.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUIRenderer
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::UiMeshRenderer16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIRenderer::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName = "ui renderer";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            UIRenderer& ui = static_cast<UIRenderer&>( _component );

            ImGui::PushID( "ui_renderer" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::FanTexturePtr( "ui texture", ui.mTexture );

                // color
                if( ImGui::Button( "##TransPos" ) )
                {
                    ui.mColor = Color::sWhite;
                }
                ImGui::SameLine();
                ImGui::ColorEdit4( "color", ui.mColor.Data(), ImGui::fanColorEditFlags );

                // uv offset
                if( ImGui::Button( "##uv_offset" ) )
                {
                    ui.mUvOffset = { 0, 0 };
                }
                ImGui::SameLine();
                ImGui::DragInt2( "uv offset", &ui.mUvOffset[0] );

                // tiling
                if( ImGui::Button( "##tiling" ) )
                {
                    ui.mTiling = { 1, 1 };
                }
                ImGui::SameLine();
                ImGui::DragInt2( "tiling", &ui.mTiling[0], 1, 1, 1000 );

                if( ImGui::Button( "##depth ui" ) ){ ui.mDepth = 0; }
                ImGui::SameLine();
                ImGui::DragInt( "depth", &ui.mDepth, 1, 0, 1024 );

                // enabled
                bool isEnabled = _world.HasTag<TagUIVisible>( _entity );
                if( ImGui::Checkbox( "visible", &isEnabled ) )
                {
                    if( isEnabled ){ _world.AddTag<TagUIVisible>( _entity ); }
                    else{ _world.RemoveTag<TagUIVisible>( _entity ); }
                }
            }
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    };
}