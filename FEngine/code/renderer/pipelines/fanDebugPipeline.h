#pragma once

namespace fan
{
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;

	//================================================================================================================================
	//================================================================================================================================
	class DebugPipeline {
	public:

		struct Uniforms
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec4 color;
		};

		DebugPipeline(Device& _device, VkRenderPass& _renderPass, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable);
		~DebugPipeline();

		void Create(VkExtent2D _extent, const char * _fragShaderPath, const char * _vertShaderPath);
		void Draw(VkCommandBuffer _commandBuffer, Buffer& _vertexBuffer, const uint32_t _count);

		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		Uniforms	GetUniforms() const { return m_uniforms; }
		void		SetUniforms(const Uniforms _uniforms);

		VkPipeline		GetPipeline() { return m_pipeline; }

	private:
		Device& m_device;
		VkRenderPass& m_renderPass;
		VkPrimitiveTopology m_primitiveTopology;

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;
		Buffer * m_uniformBuffer;
		Uniforms m_uniforms;

		bool m_depthTestEnable;

		void CreateShaders(const char * _vertShaderPath, const char * _fragShaderPath);
		bool CreateDescriptors();
		bool CreatePipeline(VkExtent2D _extent);

		void DeletePipeline();
		void DeleteDescriptors();
	};
}
