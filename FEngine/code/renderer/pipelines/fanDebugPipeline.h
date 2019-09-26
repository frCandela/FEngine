#pragma once

namespace fan
{
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	struct DebugUniforms;

	//================================================================================================================================
	//================================================================================================================================
	class DebugPipeline {
	public:
		DebugPipeline(Device& _device, VkRenderPass& _renderPass, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable);
		~DebugPipeline();

		void Create(VkExtent2D _extent, const char * _fragShaderPath, const char * _vertShaderPath);
		void Draw(VkCommandBuffer _commandBuffer, Buffer& _vertexBuffer, const uint32_t _count);
		void Resize(VkExtent2D _extent);
		void ReloadShaders();
		void SetUniformPointers( DebugUniforms * _debugUniforms	);
		void UpdateUniformBuffers();
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
		DebugUniforms  * m_debugUniforms;

		bool m_depthTestEnable;

		void CreateShaders(const char * _vertShaderPath, const char * _fragShaderPath);
		bool CreateDescriptors();
		bool CreatePipeline(VkExtent2D _extent);

		void DeletePipeline();
		void DeleteDescriptors();
	};
}
