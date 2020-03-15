#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanSingletonComponent.hpp"
#include "render/fanRenderer.hpp"

namespace fan
{
	class Mesh;

	//================================================================================================================================
	//================================================================================================================================
	class RenderWorld : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		std::vector<DrawMesh>				drawData;
		std::vector<DrawUIMesh>				uiDrawData;
		std::vector<DrawPointLight>			pointLights;
		std::vector<DrawDirectionalLight>	directionalLights;

		glm::vec2 targetSize; // render target size in pixels
	};
}