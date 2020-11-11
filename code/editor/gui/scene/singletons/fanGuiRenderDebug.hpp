#prama once

#include "scene/singletons/fanRenderDebug.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void RenderDebug::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::RenderDebug16;
		_info.mGroup = EngineGroups::SceneRender;
		_info.onGui  = &RenderDebug::OnGui;
		_info.mName  = "render debug";
	}

	//========================================================================================================
    //========================================================================================================
    void RenderDebug::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        RenderDebug& renderDebug = static_cast<RenderDebug&>( _component );
        ImGui::Text( "debugLines:            %u", renderDebug.mDebugLines.size() );
        ImGui::Text( "debugLinesNoDepthTest: %u", renderDebug.mDebugLinesNoDepthTest.size() );
        ImGui::Text( "debugTriangles:        %u", renderDebug.mDebugTriangles.size() );
        ImGui::Text( "debugLines2D:          %u", renderDebug.mDebugLines2D.size() );
    }
}