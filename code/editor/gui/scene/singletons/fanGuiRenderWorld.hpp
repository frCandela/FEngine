#prama once

#include "scene/singletons/fanRenderWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void RenderWorld::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Renderer16;
		_info.mGroup = EngineGroups::SceneRender;
		_info.onGui  = &RenderWorld::OnGui;
		_info.mName  = "render world";
	}

	//========================================================================================================
	//========================================================================================================
    void RenderWorld::OnGui( EcsWorld&, EcsSingleton& _component )
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
}