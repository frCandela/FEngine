#pragma once

#include "scene/components/fanMeshRenderer.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"


namespace fan
{
    struct GuiMeshRenderer
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::MeshRenderer16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiMeshRenderer::OnGui;
            info.mEditorPath = "/";
            info.mEditorName       = "mesh renderer";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
            ImGui::FanMeshPtr( "mesh", meshRenderer.mMesh );
        }
    };
}