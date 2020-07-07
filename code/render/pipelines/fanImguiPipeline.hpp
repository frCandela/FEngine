#pragma once

#include <vector>
#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/core/fanSampler.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanBuffer.hpp"

namespace fan
{
	class Device;
	struct ImageView;

	//================================================================================================================================
	// imgui backend
	//================================================================================================================================
	class ImguiPipeline
	{
	public:
		ImguiPipeline( Device& _device, const int _swapchainImagesCount );
		~ImguiPipeline();

		void Create( VkRenderPass _renderPass, GLFWwindow* _window, VkExtent2D _extent );
		void UpdateBuffer( const size_t _index );
		void DrawFrame( VkCommandBuffer commandBuffer, const size_t _index );
		void ReloadIcons();
		void SetGameView( ImageView* _imageView ) { m_gameImageView = _imageView; }
		void UpdateGameImageDescriptor();

	private:
		Device& m_device;

		Sampler m_sampler;
		Sampler m_iconsSampler;
		Texture m_fontTexture;
		Texture m_iconsTexture;
		ImageView * m_gameImageView;

		Shader m_fragShader;
		Shader m_vertShader;

		std::vector<Buffer>		m_vertexBuffers;
		std::vector<Buffer>		m_indexBuffers;
		std::vector < int32_t>	m_vertexCount;
		std::vector < int32_t>	m_indexCount;

		VkPipelineCache			m_pipelineCache;
		VkPipelineLayout		m_pipelineLayout;
		VkPipeline				m_pipeline;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorSet			m_descriptorSets[ 3 ];

		// UI params are set via push constants
		struct PushConstBlock
		{
			glm::vec2 scale;
			glm::vec2 translate;
		} m_pushConstBlock;

		void InitImgui( GLFWwindow* _window, VkExtent2D _extent );
		void CreateGraphicsPipeline( VkRenderPass _renderPass );
		void CreateFontAndSampler();
		void CreateDescriptors();

		static void			SetClipboardText( void* _userData, const char* _text ) { glfwSetClipboardString( ( GLFWwindow* ) _userData, _text ); }
		static const char*	GetClipboardText( void* _userData ) { return glfwGetClipboardString( ( GLFWwindow* ) _userData ); }
	};
}