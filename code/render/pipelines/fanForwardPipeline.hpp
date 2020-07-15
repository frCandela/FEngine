#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/memory/fanAlignedMemory.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	class Mesh;
	struct Vertex;
	struct Device;
	class ResourceManager;

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

	struct ForwardUniforms
	{
		void Create( Device& _device )
		{
			// Calculate required alignment based on minimum device offset alignment
			size_t minUboAlignment = (size_t)_device.mDeviceProperties.limits.minUniformBufferOffsetAlignment;
			size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
			size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
			if( minUboAlignment > 0 )
			{
				dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
				dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			}

			m_dynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
			m_dynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

			m_dynamicUniformsVert.Resize( 256 );
			m_dynamicUniformsMaterial.Resize( 256 );

			for( int uniformIndex = 0; uniformIndex < m_dynamicUniformsMaterial.Size(); uniformIndex++ )
			{
				m_dynamicUniformsMaterial[uniformIndex].color = glm::vec4( 1 );
				m_dynamicUniformsMaterial[uniformIndex].shininess = 1;
			}
		}

		AlignedMemory<DynamicUniformsMaterial>	m_dynamicUniformsMaterial;
		AlignedMemory<DynamicUniformsVert>		m_dynamicUniformsVert;
		LightsUniforms	m_lightUniforms;
		VertUniforms	m_vertUniforms;
		FragUniforms	m_fragUniforms;
	};
}