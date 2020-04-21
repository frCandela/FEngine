#include "scene/singletonComponents/fanRenderWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( RenderWorld );

	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::RENDERER16;
		_info.init = &RenderWorld::Init;
		_info.onGui = &RenderWorld::OnGui;
		_info.name = "render world";
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		RenderWorld& renderWorld = static_cast<RenderWorld&>( _component );
		renderWorld.drawData.clear();
		renderWorld.uiDrawData.clear();
		renderWorld.pointLights.clear();
		renderWorld.directionalLights.clear();
		renderWorld.targetSize = {1920,1080};
		if( !renderWorld.particlesMesh.GetVertices().empty() )
		{
			renderWorld.particlesMesh = Mesh();
		}
		renderWorld.particlesMesh.SetHostVisible( true );
		renderWorld.particlesMesh.SetOptimizeVertices( false );
		renderWorld.particlesMesh.SetAutoUpdateHull( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		RenderWorld& renderWorld = static_cast<RenderWorld&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "mesh:               %d", renderWorld.drawData.size() );
			ImGui::Text( "ui mesh:            %d", renderWorld.uiDrawData.size() );
			ImGui::Text( "pointLights:        %d", renderWorld.pointLights.size() );
			ImGui::Text( "directionalLights:  %d", renderWorld.directionalLights.size() );
			ImGui::Text( "particles vertices: %d", renderWorld.particlesMesh.GetVertices().size() );
			ImGui::Text( "target size:        %d x %d", (int)renderWorld.targetSize.x, (int)renderWorld.targetSize.y );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}