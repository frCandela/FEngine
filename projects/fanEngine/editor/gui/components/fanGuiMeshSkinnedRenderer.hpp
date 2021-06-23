#pragma once

#include "engine/components/fanSkinnedMeshRenderer.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSkinnedMeshRenderer
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::MeshRenderer16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiSkinnedMeshRenderer::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "Skinned Mesh Renderer";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            SkinnedMeshRenderer& meshRenderer = static_cast<SkinnedMeshRenderer&>( _component );
            ImGui::FanResourcePtr<SkinnedMesh>( "mesh", meshRenderer.mMesh );
        }
    };
}