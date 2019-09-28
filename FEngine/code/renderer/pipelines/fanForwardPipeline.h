#pragma once

#include "renderer/fanUniforms.h"
#include "core/memory/fanAlignedMemory.h"

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
			AlignedMemory<DynamicUniformsMaterial>* _dynamicUniformsMaterial,
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

		Descriptor * m_sceneDescriptor;
		Descriptor * m_texturesDescriptor;

		Sampler *	m_sampler;
		Image *		m_depthImage;
		ImageView *	m_depthImageView;

		Shader *	m_fragmentShader = nullptr;
		Shader *	m_vertexShader = nullptr;

		// Pointer to the uniforms data
		AlignedMemory<DynamicUniformsVert>*		m_dynamicUniformsVert;
		VertUniforms * m_vertUniforms;
 		AlignedMemory<DynamicUniformsMaterial>* m_dynamicUniformsMaterial;
 		LightsUniforms * m_lightUniforms;
 		FragUniforms * m_fragUniforms;

		void CreateShaders(); 
		bool CreateDescriptors();
		bool CreateDepthRessources(VkExtent2D _extent);
		bool CreatePipeline(VkExtent2D _extent);

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}