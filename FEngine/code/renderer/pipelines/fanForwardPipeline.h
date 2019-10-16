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
		glm::vec3  color = glm::vec3( 1 );
		glm::int32 shininess;
		glm::int32 textureIndex;
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
		void UpdateUniformBuffers( const size_t _index = 0 ) override;
		void CreateDescriptors( const size_t _numSwapchainImages, RessourceManager * const _ressourceManager );
		void ReloadShaders() override;

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor * m_sceneDescriptor;
		Descriptor * m_texturesDescriptor;		

		Sampler *			m_sampler = nullptr;
		RessourceManager *	m_ressourceManager = nullptr;

		bool CreateTextureDescriptor();
		void SetTextureDescriptor( const int _index = -1 );
	};
}