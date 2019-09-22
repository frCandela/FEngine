#pragma once

namespace fan
{
	class Device;
	class Shader;
	class Sampler;
	class Image;
	class ImageView;
	class Buffer;

	//================================================================================================================================
	//================================================================================================================================
	class PostprocessPipeline {
	public:
		PostprocessPipeline(Device& _device, VkRenderPass& _renderPass);
		~PostprocessPipeline();

		struct Uniforms {
			glm::vec4 color;
		};

		bool Create(const VkFormat _format, VkExtent2D _extent);
		void Draw(VkCommandBuffer _commandBuffer);
		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		Uniforms GetUniforms() const { return m_uniforms; }
		void SetUniforms(const Uniforms _uniforms);

		VkPipeline		GetPipeline() { return m_pipeline; }
		VkImageView		GetImageView();

	private:
		void CreateShaders();
		bool CreateDescriptors();
		bool CreatePipeline(VkExtent2D _extent);
		void CreateImagesAndViews(VkExtent2D _extent);
		void CreateVertexBuffer();

		void DeleteImages();
		void DeletePipeline();
		void DeleteDescriptors();

		Device& m_device;
		VkRenderPass& m_renderPass;
		VkFormat m_format;


		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		Sampler *		m_sampler;
		Image *			m_image;
		ImageView *		m_imageView;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		Buffer * m_uniformBuffer;
		Buffer * m_vertexBuffer;

		Uniforms m_uniforms;
	};
}