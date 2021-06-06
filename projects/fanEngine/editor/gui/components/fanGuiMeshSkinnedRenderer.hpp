#pragma once

#include "engine/components/fanMeshSkinnedRenderer.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiMeshSkinnedRenderer
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::MeshRenderer16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiMeshSkinnedRenderer::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "mesh renderer (skinned)";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            MeshSkinnedRenderer& meshRenderer = static_cast<MeshSkinnedRenderer&>( _component );
            ImGui::FanMeshSkinnedPtr( "mesh", meshRenderer.mMesh );
        }
    };
}