#pragma once

namespace vk {
	class Device;
	class Shader;
	class Sampler;
	class Image;
	class ImageView;
	class Buffer;

	class PostprocessPipeline {
	public:
		PostprocessPipeline(Device * _device);
		~PostprocessPipeline();

		struct UniformsPostprocess {
			glm::vec4 color;
		};

		bool Create( const VkFormat _format, VkExtent2D _extent);
		void ReloadShaders();

		UniformsPostprocess GetUniforms() const { return m_uniformsPostprocess; }
		void SetUniforms(const UniformsPostprocess _uniforms);
		
		VkRenderPass	GetRenderPass() { return m_renderPassPostprocess; }
		VkPipeline		GetPipeline()	{ return m_pipelinePostprocess; }
		VkImageView		GetImageView();

		void RecordCommandBufferPostProcess(VkCommandBuffer _commandBuffer, VkFramebuffer _framebuffer);

	private:
		void CreateShaders();
		bool CreateDescriptorsPostprocess();
		bool CreateRenderPassPostprocess( const VkFormat _format );
		bool CreatePipelinePostprocess( VkExtent2D _extent );
		void CreatePostprocessImages( const VkFormat _format, VkExtent2D _extent);
		void CreatePostprocessVertexBuffer();

		void DeleteRenderPassPostprocess();
		void DeletePostprocessRessources();
		void DeletePipelinePostprocess();
		void DeleteDescriptorsPostprocess();

		Device * const m_device;

		VkRenderPass	m_renderPassPostprocess;
		VkPipelineLayout	m_pipelineLayoutPostprocess;
		VkPipeline			m_pipelinePostprocess;

		Shader * m_fragmentShaderPostprocess = nullptr;
		Shader * m_vertexShaderPostprocess = nullptr;

		Sampler *		m_samplerPostprocess;
		Image *			m_imagePostprocess;
		ImageView *		m_imageViewPostprocess;

		VkDescriptorSetLayout	m_descriptorSetLayoutPostprocess;
		VkDescriptorPool		m_descriptorPoolPostprocess;
		VkDescriptorSet			m_descriptorSetPostprocess;

		Buffer * m_uniformBufferPostprocess;
		Buffer * m_vertexBufferPostprocess;

		UniformsPostprocess m_uniformsPostprocess;
	};
}