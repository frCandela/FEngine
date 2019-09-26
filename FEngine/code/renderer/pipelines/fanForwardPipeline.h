#pragma once

#include "renderer/fanUniforms.h"
#include "core/memory/fanAlignedMemory.h"

namespace fan
{
	class Mesh;
	struct Vertex;
	struct MeshData;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	class Texture;
	class Sampler;
	struct DrawData;

	//================================================================================================================================
	//================================================================================================================================
	class ForwardPipeline {
	public:
		ForwardPipeline(Device& _device, VkRenderPass& _renderPass);
		~ForwardPipeline();

		void Create(VkExtent2D _extent);
		void Draw(VkCommandBuffer _commandBuffer, const std::array< Mesh *, s_maximumNumModels > _meshArray, const uint32_t _num );
		void Resize(VkExtent2D _extent);
		void ReloadShaders();
		void UpdateUniformBuffers();
		void SetUniformPointers( 
			LightsUniforms * _lightUniforms, 
			AlignedMemory<DynamicUniformsVert>* _dynamicUniformsVert, 
			AlignedMemory<DynamicUniformsMaterial>* _dynamicUniformsFrag ,
			VertUniforms * _vertUniforms,
			FragUniforms * _fragUniforms
		);

		VkPipeline		GetPipeline() { return m_pipeline; }
		VkImageView		GetDepthImageView();

	private:
		Device&					m_device;
		VkRenderPass&			m_renderPass;

		VkPipelineLayout		m_pipelineLayout;
		VkPipeline				m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayoutScene;
		VkDescriptorPool		m_descriptorPoolScene;
		VkDescriptorSet			m_descriptorSetScene;

		VkDescriptorSetLayout	m_descriptorSetLayoutTextures;
		VkDescriptorPool		m_descriptorPoolTextures;
		VkDescriptorSet			m_descriptorSetTextures;

		Sampler *	m_sampler;
		Image *		m_depthImage;
		ImageView *	m_depthImageView;

		Shader *	m_fragmentShader = nullptr;
		Shader *	m_vertexShader = nullptr;

		Buffer *	m_vertUniformBuffer;
		Buffer *	m_fragUniformBuffer;
		Buffer *	m_pointLightUniformBuffer;
		Buffer *	m_dynamicUniformBufferVert;
		Buffer *	m_dynamicUniformBufferFrag;

		// Pointer to the uniforms data
		AlignedMemory<DynamicUniformsVert>*		m_dynamicUniformsVert;
		AlignedMemory<DynamicUniformsMaterial>* m_dynamicUniformsFrag;
		LightsUniforms * m_lightUniforms;
		VertUniforms * m_vertUniforms;
		FragUniforms * m_fragUniforms;

		void CreateShaders(); 
		bool CreateDescriptors();
		bool CreateDescriptorsScene(  );
		bool CreateDescriptorsTextures();
		bool CreateDepthRessources(VkExtent2D _extent);
		bool CreatePipeline(VkExtent2D _extent);

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}