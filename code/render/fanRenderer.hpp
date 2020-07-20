#pragma once

#include <vector>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
WARNINGS_POP()
#include "glfw/glfw3.h"
#include "core/math/fanVector2.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanCommandBuffer.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanImageView.hpp"
#include "render/core/fanImage.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/draw/fanDrawImgui.hpp"
#include "render/draw/fanDrawModels.hpp"
#include "render/draw/fanDrawDebug.hpp"
#include "render/draw/fanDrawUI.hpp"
#include "render/draw/fanDrawPostprocess.hpp"

namespace fan
{
	class Window;

	struct RenderDataDirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 ambiant;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	struct RenderDataPointLight
	{
		glm::vec4	 position;
		glm::vec4	 diffuse;
		glm::vec4	 specular;
		glm::vec4	 ambiant;
		glm::float32 constant;
		glm::float32 linear;
		glm::float32 quadratic;
	};

	struct RenderDataModel
	{
		Mesh* mesh;
		glm::mat4 modelMatrix;
		glm::mat4 normalMatrix;
		glm::vec4 color;
		uint32_t shininess;
		uint32_t textureIndex;
	};

	struct RenderDataUIMesh
	{
		UIMesh* mesh;
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
		uint32_t textureIndex;
	};	   

	//================================================================================================================================
	// Contains all the rendering data
	//================================================================================================================================
	class Renderer
	{
	public:
		Renderer( Window& _window );
		~Renderer();

		void DrawFrame();
		void WaitIdle();

		void ReloadIcons();
		void ReloadShaders();
		void ResizeGame( btVector2 _newSize );
		void ResizeSwapchain();

		void SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position );
		void SetDirectionalLights( const std::vector<RenderDataDirectionalLight>& _lightData );
		void SetPointLights( const std::vector<RenderDataPointLight>& _lightData );
		void SetDrawData( const std::vector<RenderDataModel>& _drawData );
		void SetUIDrawData( const std::vector<RenderDataUIMesh>& _drawData );
		void SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles );

		Window& mWindow;
		Device& mDevice;

		VkExtent2D		mGameExtent = { 1,1 };
		glm::vec4		mClearColor = glm::vec4( 0.f, 0.f, 0.2f, 1.f );;
		CommandBuffer	mPrimaryCommandBuffers;

		// data
		DrawModels		mDrawModels;
		DrawDebug		mDrawDebug;
		DrawUI			mDrawUI;
		DrawPostprocess	mDrawPostprocess;
		DrawImgui		mDrawImgui;	

		// global descriptors
		DescriptorImages	mDescriptorTextures;

		// render passes
		RenderPass mRenderPassGame;
		RenderPass mRenderPassPostprocess;
		RenderPass mRenderPassImgui;

		// frame buffers game 
		FrameBuffer mFrameBuffersGame;
		Image		mImageGameDepth;
		ImageView	mImageViewGameDepth;
		Sampler		mSamplerGameColor;
		Image		mImageGameColor;
		ImageView	mImageViewGameColor;

		// frame buffers postprocess 
		FrameBuffer mFramebuffersPostprocess;
		Sampler		mSamplerPostprocessColor;
		Image		mImagePostprocessColor;
		ImageView	mImageViewPostprocessColor;

		// frame buffers swapchain 
		FrameBuffer mFramebuffersSwapchain;

		void RecordPrimaryCommandBuffer( const size_t _index );
		void RecordAllCommandBuffers();

		void CreateCommandBuffers();
		void CreateFramebuffers( const VkExtent2D _extent );
		void CreateRenderPasses();
		void CreateTextureDescriptor();
		void CreateShaders();
		void CreatePipelines();
		void DestroyShaders();
		void DestroyPipelines();

		void UpdateUniformBuffers( Device& _device, const size_t _index );
		void SubmitCommandBuffers();
	};
}
