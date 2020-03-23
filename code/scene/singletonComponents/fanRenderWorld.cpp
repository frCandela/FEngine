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
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWorld::Init( SingletonComponent& _component )
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
}