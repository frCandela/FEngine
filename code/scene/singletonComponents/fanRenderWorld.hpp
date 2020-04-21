#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	// contains all the render data of the world for the renderer
	//================================================================================================================================
	class RenderWorld : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		std::vector<DrawMesh>				drawData;
		std::vector<DrawUIMesh>				uiDrawData;
		std::vector<DrawPointLight>			pointLights;
		std::vector<DrawDirectionalLight>	directionalLights;

		Mesh particlesMesh;

		glm::vec2 targetSize; // render target size in pixels
	};
}