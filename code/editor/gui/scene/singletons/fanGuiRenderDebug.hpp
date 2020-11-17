#pragma once

#include "engine/singletons/fanRenderDebug.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiRenderDebug
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::RenderDebug16;
            info.mGroup = EngineGroups::SceneRender;
            info.onGui  = &GuiRenderDebug::OnGui;
            info.mEditorName  = "render debug";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            RenderDebug& renderDebug = static_cast<RenderDebug&>( _component );
            ImGui::Text( "debugLines:            %u", renderDebug.mDebugLines.size() );
            ImGui::Text( "debugLinesNoDepthTest: %u", renderDebug.mDebugLinesNoDepthTest.size() );
            ImGui::Text( "debugTriangles:        %u", renderDebug.mDebugTriangles.size() );
            ImGui::Text( "debugLines2D:          %u", renderDebug.mDebugLines2D.size() );
        }
    };
}