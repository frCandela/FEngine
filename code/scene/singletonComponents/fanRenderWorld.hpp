#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
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
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );

		std::vector<DrawMesh>				drawData;
		std::vector<DrawUIMesh>				uiDrawData;
		std::vector<DrawPointLight>			pointLights;
		std::vector<DrawDirectionalLight>	directionalLights;

		Mesh particlesMesh;

		glm::vec2 targetSize; // render target size in pixels
	};
}