#pragma once

#include <vector>
#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/fanShader.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanCommandBuffer.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"

namespace fan
{
	struct Device;
	struct ImageView;

    //========================================================================================================
    //========================================================================================================
	struct PushConstBlock
	{
		glm::vec2 scale;
		glm::vec2 translate;
	};

	struct RenderPass;
	struct FrameBuffer;

	//========================================================================================================
	// imgui backend
	//========================================================================================================
	struct DrawImgui
	{
		Pipeline			mPipeline;
		Sampler				mSampler;
		Sampler				mSamplerIcons;
		Texture				mTextureFont;
		Texture				mTextureIcons;
		Shader				mFragmentShader;
		Shader				mVertexShader;
		DescriptorImages	mDescriptorImages;
		CommandBuffer		mCommandBuffers;
		PushConstBlock		mPushConstBlock;

		std::vector<Buffer>		mVertexBuffers;
		std::vector<Buffer>		mIndexBuffers;
		std::vector < int32_t>	mVertexCount;
		std::vector < int32_t>	mIndexCount;

        void Create( Device& _device, const int _swapchainImagesCount,
                     VkRenderPass _renderPass,
                     GLFWwindow* _window,
                     VkExtent2D _extent,
                     ImageView& _gameImageView );
		void Destroy( Device& _device );
		
		void UpdateBuffer( Device& _device, const size_t _index );
		void DrawFrame( VkCommandBuffer commandBuffer, const size_t _index );
		void ReloadIcons( Device& _device );
		void UpdateGameImageDescriptor( Device& _device, ImageView& _gameImageView );
        void RecordCommandBuffer( const size_t _index,
                                  Device& _device,
                                  RenderPass& _renderPass,
                                  FrameBuffer& _framebuffer );

		static void			SetClipboardText( void* _userData, const char* _text );
		static const char*	GetClipboardText( void* _userData );
		
	private:
		void InitImgui( Device& _device, GLFWwindow* _window, VkExtent2D _extent );
		PipelineConfig	GetPipelineConfig() const;
	};
}