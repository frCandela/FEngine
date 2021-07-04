#pragma once

#include "engine/components/fanMeshRenderer.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiMeshRenderer
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::MeshRenderer16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiMeshRenderer::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "Mesh renderer";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
            ImGui::FanResourcePtr<Mesh>( "mesh", meshRenderer.mMesh );
            if( ImGui::Checkbox( "transparent", &meshRenderer.mTransparent )  )
            {
                if( meshRenderer.mTransparent )
                {
                    _world.AddTag<TagTransparent>( _entity );
                }
                else
                {
                    _world.RemoveTag<TagTransparent>( _entity );
                }
            }
        }
    };
}