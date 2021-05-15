#pragma once

#include "engine/singletons/fanRenderWorld.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiRenderWorld
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Renderer16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiRenderWorld::OnGui;
            info.mEditorName = "render world";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _singleton )
        {
            RenderWorld& renderWorld = static_cast<RenderWorld&>( _singleton );
            ImGui::Text( "mesh:               %d", (int)renderWorld.drawData.size() );
            ImGui::Text( "ui mesh:            %d", (int)renderWorld.uiDrawData.size() );
            ImGui::Text( "pointLights:        %d", (int)renderWorld.pointLights.size() );
            ImGui::Text( "directionalLights:  %d", (int)renderWorld.directionalLights.size() );
            ImGui::Text( "particles vertices: %d", (int)renderWorld.mParticlesMesh->mSubMeshes[0].mVertices.size() );
            ImGui::Text( "target size:        %d x %d", (int)renderWorld.mTargetSize.x, (int)renderWorld.mTargetSize.y );
            ImGui::Checkbox("fullscreen", &renderWorld.mFullscreen.mIsFullScreen );
            ImGui::PushReadOnly();
            ImGui::Checkbox( "is headless", &renderWorld.mIsHeadless );
            ImGui::PopReadOnly();
        }
    };
}