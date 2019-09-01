#pragma once

#include "fanIncludes.h"

namespace fan
{
	namespace vk
	{
		class Device;
		class Shader;
		class Buffer;
		class Texture;
		class Sampler;

		//================================================================================================================================
		//================================================================================================================================
		class ImguiPipeline
		{
		public:
			ImguiPipeline(Device& _device, const int _swapchainImagesCount);
			~ImguiPipeline();

			void Create(VkRenderPass _renderPass, GLFWwindow* _window, VkExtent2D _extent);
			void UpdateBuffer(const int _index);
			void DrawFrame(VkCommandBuffer commandBuffer, const int _index);

		private:
			Device & m_device;

			Sampler * m_sampler;
			Texture * m_fontTexture;

			Shader * m_fragShader;
			Shader * m_vertShader;

			std::vector<Buffer>		m_vertexBuffers;
			std::vector<Buffer>		m_indexBuffers;
			std::vector < int32_t>	m_vertexCount;
			std::vector < int32_t>	m_indexCount;

			VkPipelineCache			m_pipelineCache;
			VkPipelineLayout		m_pipelineLayout;
			VkPipeline				m_pipeline;
			VkDescriptorPool		m_descriptorPool;
			VkDescriptorSetLayout	m_descriptorSetLayout;
			VkDescriptorSet			m_descriptorSet;

			// UI params are set via push constants
			struct PushConstBlock
			{
				glm::vec2 scale;
				glm::vec2 translate;
			} m_pushConstBlock;

			void InitImgui(GLFWwindow* _window, VkExtent2D _extent);
			void CreateGraphicsPipeline(VkRenderPass _renderPass);
			void CreateFontAndSampler();
			void CreateDescriptors();

			static void			SetClipboardText(void* _userData, const char* _text) { glfwSetClipboardString((GLFWwindow*)_userData, _text); }
			static const char*	GetClipboardText(void* _userData) { return glfwGetClipboardString((GLFWwindow*)_userData); }
		};
	}
}