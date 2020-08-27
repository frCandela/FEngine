#pragma once

#include "bullet/LinearMath/btTransform.h"
#include "core/shapes/fanAABB.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
	//========================================================================================================
	// contains all the render data of the world for the renderer
	//========================================================================================================
	class RenderWorld : public EcsSingleton
	{
		ECS_SINGLETON( RenderWorld )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		std::vector<RenderDataModel>            drawData;
		std::vector<RenderDataMesh2D>           uiDrawData;
		std::vector<RenderDataPointLight>       pointLights;
		std::vector<RenderDataDirectionalLight> directionalLights;

		Mesh*       particlesMesh = nullptr;
		glm::vec2   targetSize; // render target size in pixels
		bool        isHeadless;
	};
}