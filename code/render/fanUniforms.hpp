#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "glfw/glfw3.h"
#include "core/memory/fanAlignedMemory.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================
	//================================================================
	struct DirectionalLightUniform
	{
		alignas( 16 ) glm::vec4 direction;
		alignas( 16 ) glm::vec4 ambiant;
		alignas( 16 ) glm::vec4 diffuse;
		alignas( 16 ) glm::vec4 specular;
	};

	//================================================================
	//================================================================
	struct PointLightUniform
	{
		alignas( 16 ) glm::vec4		position;
		alignas( 16 ) glm::vec4		diffuse;
		alignas( 16 ) glm::vec4		specular;
		alignas( 16 ) glm::vec4		ambiant;
		alignas( 4 ) glm::float32	constant;
		alignas( 4 ) glm::float32	linear;
		alignas( 4 ) glm::float32	quadratic;
		alignas( 4 ) glm::float32	_0;
	};

	//================================================================
	//================================================================
	struct LightsUniforms
	{
		DirectionalLightUniform dirLights[ RenderGlobal::s_maximumNumDirectionalLight ];
		PointLightUniform		pointlights[ RenderGlobal::s_maximumNumPointLights ];
		uint32_t				dirLightsNum;
		uint32_t				pointLightNum;
	};

	//================================================================
	//================================================================
	struct DynamicUniformsVert
	{
		glm::mat4 modelMat;
		glm::mat4 normalMat;
	};

	//================================================================
	//================================================================
	struct DynamicUniformsMaterial
	{
		glm::vec4  color = glm::vec4( 1 );
		glm::int32 shininess;
	};

	//================================================================
	//================================================================
	struct VertUniforms
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	//================================================================
	//================================================================
	struct FragUniforms
	{
		glm::vec3	cameraPosition = glm::vec3( 0, 0, 0 );
	};

	//================================================================
	//================================================================
	struct DynamicUniformUIVert
	{
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
	};

	//================================================================
	//================================================================
	struct ForwardUniforms
	{
		void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

		AlignedMemory<DynamicUniformsMaterial>	m_dynamicUniformsMaterial;
		AlignedMemory<DynamicUniformsVert>		m_dynamicUniformsVert;
		LightsUniforms	m_lightUniforms;
		VertUniforms	m_vertUniforms;
		FragUniforms	m_fragUniforms;
	};

	//================================================================
	//================================================================
	struct UiUniforms
	{
		void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

		AlignedMemory<DynamicUniformUIVert>	mUidynamicUniformsVert;
	};

	//================================================================
	//================================================================
	struct DebugUniforms
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 color;
	};

	//================================================================
	//================================================================
	struct PostprocessUniforms
	{
		glm::vec4 color = glm::vec4( 1, 1, 1, 1 );
	};
}