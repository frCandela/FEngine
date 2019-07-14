#pragma once

#include "util/fanAlignedMemory.h"

namespace scene { 
	class Mesh;  
	class Transform;
}

namespace vk {
	struct Vertex;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;


	struct MeshData {
		scene::Mesh * mesh;
		scene::Transform * transform;
		vk::Buffer * indexBuffer;
		vk::Buffer * vertexBuffer;
	};

	class ForwardPipeline {
	public:

		struct Uniforms
		{
			glm::mat4 view;
			glm::mat4 proj;
		};
		struct DynamicUniforms
		{
			glm::mat4 modelMat;
			glm::mat4 rotationMat;
		};


		ForwardPipeline(Device& _device, VkRenderPass& _renderPass);
		~ForwardPipeline();

		void Create( VkExtent2D _extent );
		void Draw(VkCommandBuffer _commandBuffer, const std::vector<MeshData>& _meshData );
		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		Uniforms GetUniforms() const { return m_uniforms; }
		void SetUniforms(const Uniforms _uniforms);
		void SetDynamicUniforms( const std::vector<DynamicUniforms> & _dynamicUniforms );

		VkPipeline		GetPipeline() { return m_pipeline; }
		VkImageView		GetDepthImageView();

	private:
		Device& m_device;
		VkRenderPass& m_renderPass;

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		Image *			m_depthImage;
		ImageView  *	m_depthImageView;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		Buffer * m_dynamicUniformBuffer;
		Buffer * m_uniformBuffer;
		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;

		Uniforms m_uniforms;
		util::AlignedMemory<DynamicUniforms> m_dynamicUniformsArray;
		size_t m_dynamicAlignment;

		void CreateShaders();
		bool CreateDescriptors();
		bool CreateDepthRessources(	VkExtent2D _extent);
		bool CreatePipeline(		VkExtent2D _extent );

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}
