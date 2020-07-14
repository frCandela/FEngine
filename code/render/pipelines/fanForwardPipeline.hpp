#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/memory/fanAlignedMemory.hpp"
#include "render/pipelines/fanPipeline.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"

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

	//================================================================================================================================
	// 3D mesh rendering  
	//================================================================================================================================
	class ForwardPipeline : public Pipeline
	{
	public:
		// Uniforms
		AlignedMemory<DynamicUniformsMaterial>	m_dynamicUniformsMaterial;
		AlignedMemory<DynamicUniformsVert>		m_dynamicUniformsVert;

		LightsUniforms	m_lightUniforms;
		VertUniforms	m_vertUniforms;
		FragUniforms	m_fragUniforms;

		ForwardPipeline( Device& _device, DescriptorImages* _textures, DescriptorSampler* _sampler );
		void Destroy( Device& _device );

		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void SetUniformsData( Device& _device, const size_t _index = 0 ) override;
		void CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages );
		void ResizeDynamicDescriptors( Device& _device, const uint32_t _count, const size_t _newSize );

		PipelineConfig GetConfig() override;

	private:
		DescriptorUniforms m_sceneDescriptor;

		DescriptorImages* m_textures;
		DescriptorSampler* m_sampler;
	};
}