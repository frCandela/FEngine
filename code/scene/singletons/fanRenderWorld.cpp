#include "scene/singletons/fanRenderWorld.hpp"

#include "editor/fanModals.hpp"
#include "scene/singletons/fanRenderResources.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::RENDERER16;
		_info.group = EngineGroups::SceneRender;
		_info.onGui = &RenderWorld::OnGui;
		_info.name = "render world";
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		RenderWorld& renderWorld = static_cast<RenderWorld&>( _component );
		renderWorld.drawData.clear();
		renderWorld.uiDrawData.clear();
		renderWorld.pointLights.clear();
		renderWorld.directionalLights.clear();
		renderWorld.targetSize = {1920,1080};
		renderWorld.particlesMesh = new Mesh();
		renderWorld.particlesMesh->mHostVisible = true;
		renderWorld.particlesMesh->mOptimizeVertices = false;
		renderWorld.particlesMesh->mAutoUpdateHull = false;
		renderWorld.isHeadless = false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		RenderWorld& renderWorld = static_cast<RenderWorld&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "mesh:               %d", renderWorld.drawData.size() );
			ImGui::Text( "ui mesh:            %d", renderWorld.uiDrawData.size() );
			ImGui::Text( "pointLights:        %d", renderWorld.pointLights.size() );
			ImGui::Text( "directionalLights:  %d", renderWorld.directionalLights.size() );
			ImGui::Text( "particles vertices: %d", renderWorld.particlesMesh->mVertices.size() );
			ImGui::Text( "target size:        %d x %d", (int)renderWorld.targetSize.x, (int)renderWorld.targetSize.y );


			ImGui::PushReadOnly();
			ImGui::Checkbox( "is headless",&renderWorld.isHeadless );
			ImGui::PopReadOnly();
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}