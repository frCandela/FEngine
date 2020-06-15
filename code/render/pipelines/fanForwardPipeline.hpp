#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/memory/fanAlignedMemory.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	class Mesh;
	struct Vertex;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	class Texture;
	class Sampler;
	class Descriptor;
	class DescriptorTextures;
	class DescriptorSampler;
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

		ForwardPipeline( Device& _device, DescriptorTextures*& _textures, DescriptorSampler*& _sampler );
		~ForwardPipeline() override;

		void Resize( const VkExtent2D _extent ) override;
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void UpdateUniformBuffers( const size_t _index = 0 ) override;
		void CreateDescriptors( const size_t _numSwapchainImages );
		void ResizeDynamicDescriptors( const size_t _newSize );
		void ReloadShaders() override;

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor* m_sceneDescriptor;

		DescriptorTextures*& m_textures;
		DescriptorSampler*& m_sampler;
	};
}