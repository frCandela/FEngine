#pragma once

#include "core/memory/fanAlignedMemory.h"

#include "renderer/core/fanPipeline.h"

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
	class RessourceManager;

	//================================================================
	//================================================================
	struct DirectionalLightUniform {
		alignas( 16 ) glm::vec4 direction;
		alignas( 16 ) glm::vec4 ambiant;
		alignas( 16 ) glm::vec4 diffuse;
		alignas( 16 ) glm::vec4 specular;
	};

	//================================================================
	//================================================================
	struct PointLightUniform {
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
	struct LightsUniforms {
		DirectionalLightUniform dirLights[GlobalValues::s_maximumNumDirectionalLight];
		PointLightUniform		pointlights[GlobalValues::s_maximumNumPointLights];
		uint32_t				dirLightsNum;
		uint32_t				pointLightNum;
	};

	//================================================================
	//================================================================
	struct DynamicUniformsVert {
		glm::mat4 modelMat;
		glm::mat4 normalMat;
	};

	//================================================================
	//================================================================
	struct DynamicUniformsMaterial {
		glm::vec4  color = glm::vec4( 1 );
		glm::int32 shininess;
	};

	//================================================================
	//================================================================
	struct VertUniforms {
		glm::mat4 view;
		glm::mat4 proj;
	};

	//================================================================
	//================================================================
	struct FragUniforms {
		glm::vec3	cameraPosition = glm::vec3( 0, 0, 0 );
	};

	//================================================================================================================================
	//================================================================================================================================
	class ForwardPipeline : public Pipeline {
	public:
		// Uniforms
		AlignedMemory<DynamicUniformsMaterial>	m_dynamicUniformsMaterial;
		AlignedMemory<DynamicUniformsVert>		m_dynamicUniformsVert;

		LightsUniforms	m_lightUniforms;
		VertUniforms	m_vertUniforms;
		FragUniforms	m_fragUniforms;

		ForwardPipeline( Device& _device );
		~ForwardPipeline() override;

		void Resize( const VkExtent2D _extent ) override;
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex );
		void UpdateUniformBuffers( const size_t _index = 0 ) override;
		void CreateDescriptors( const size_t _numSwapchainImages, RessourceManager * const _ressourceManager );
		void ResizeDynamicDescriptors ( const size_t _newSize );
		void ReloadShaders() override;
		bool CreateTextureDescriptor();

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor *		 m_sceneDescriptor;
		DescriptorTextures * m_texturesDescriptor;

		Sampler *			m_sampler = nullptr;
		RessourceManager *	m_ressourceManager = nullptr;


	};
}