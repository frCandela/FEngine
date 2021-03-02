#pragma once

#include "engine/singletons/fanRenderWorld.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiRenderWorld
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Renderer16;
            info.mGroup = EngineGroups::SceneRender;
            info.onGui  = &GuiRenderWorld::OnGui;
            info.mEditorName  = "render world";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            RenderWorld& renderWorld = static_cast<RenderWorld&>( _component );
            ImGui::Text( "mesh:               %d", renderWorld.drawData.size() );
            ImGui::Text( "ui mesh:            %d", renderWorld.uiDrawData.size() );
            ImGui::Text( "pointLights:        %d", renderWorld.pointLights.size() );
            ImGui::Text( "directionalLights:  %d", renderWorld.directionalLights.size() );
            ImGui::Text( "particles vertices: %d", renderWorld.mParticlesMesh->mVertices.size() );
            ImGui::Text( "target size:        %d x %d",
                         (int)renderWorld.mTargetSize.x,
                         (int)renderWorld.mTargetSize.y );

            ImGui::PushReadOnly();
            ImGui::Checkbox( "is headless", &renderWorld.mIsHeadless );
            ImGui::PopReadOnly();
        }
    };
}