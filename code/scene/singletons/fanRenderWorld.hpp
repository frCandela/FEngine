#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	// contains all the render data of the world for the renderer
	//================================================================================================================================
	class RenderWorld : public EcsSingleton
	{
		ECS_SINGLETON( RenderWorld )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		std::vector<DrawMesh>				drawData;
		std::vector<DrawUIMesh>				uiDrawData;
		std::vector<DrawPointLight>			pointLights;
		std::vector<DrawDirectionalLight>	directionalLights;

		Mesh particlesMesh;

		glm::vec2 targetSize; // render target size in pixels
	};
}