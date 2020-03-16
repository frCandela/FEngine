#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanSingletonComponent.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class RenderWorld : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		RenderWorld();

		std::vector<DrawMesh>				drawData;
		std::vector<DrawUIMesh>				uiDrawData;
		std::vector<DrawPointLight>			pointLights;
		std::vector<DrawDirectionalLight>	directionalLights;

		Mesh particlesMesh;

		glm::vec2 targetSize; // render target size in pixels
	};
}