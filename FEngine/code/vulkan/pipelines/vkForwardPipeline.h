#pragma once

namespace vk {
	struct Vertex;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;

	class ForwardPipeline {
	public:

		struct Uniforms
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};

		ForwardPipeline(Device& _device, VkRenderPass& _renderPass);
		~ForwardPipeline();

		void Create( VkExtent2D _extent );
		void Draw(VkCommandBuffer _commandBuffer);
		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		Uniforms GetUniforms() const { return m_uniforms; }
		void SetUniforms(const Uniforms _uniforms);

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

		Buffer * m_uniformBuffer;
		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;

		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;

		Uniforms m_uniforms;

		void CreateShaders();
		bool CreateDescriptors();
		bool CreateDepthRessources(	VkExtent2D _extent);
		bool CreatePipeline(		VkExtent2D _extent );
		void CreateVertexBuffers();

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}
