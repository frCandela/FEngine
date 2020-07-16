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
#include "render/fanUniforms.hpp"

#include "render/draw/fanDrawImgui.hpp"
#include "render/draw/fanDrawModels.hpp"
#include "render/draw/fanDrawDebug.hpp"
#include "render/draw/fanDrawUI.hpp"
#include "render/draw/fanDrawPostprocess.hpp"

namespace fan
{
	class Window;
	class UIMesh;

	// Used to set uniforms
	struct DrawDirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 ambiant;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	struct DrawPointLight
	{
		glm::vec4	 position;
		glm::vec4	 diffuse;
		glm::vec4	 specular;
		glm::vec4	 ambiant;
		glm::float32 constant;
		glm::float32 linear;
		glm::float32 quadratic;
	};

	struct DrawMesh
	{
		Mesh* mesh;
		glm::mat4 modelMatrix;
		glm::mat4 normalMatrix;
		glm::vec4 color;
		uint32_t shininess;
		uint32_t textureIndex;
	};

	struct DrawUIMesh
	{
		UIMesh* mesh;
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
		uint32_t textureIndex;
	};

	// Used to batch rendering
	struct UIDrawData
	{
		UIMesh* mesh;
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
		void SetDirectionalLights( const std::vector<DrawDirectionalLight>& _lightData );
		void SetPointLights( const std::vector<DrawPointLight>& _lightData );
		void SetDrawData( const std::vector<DrawMesh>& _drawData );
		void SetUIDrawData( const std::vector<DrawUIMesh>& _drawData );
		void SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles );

		VkExtent2D m_gameExtent = { 1,1 };		
		std::vector< UIDrawData >	m_uiMeshDrawArray;

		Window& m_window;
		Device& m_device;

		// data
		DrawModels		mDrawModels;
		DrawDebug		mDrawDebug;
		DrawUI			mDrawUI;
		DrawPostprocess	mDrawPostprocess;
		DrawImgui		mDrawImgui;

		glm::vec4	mClearColor;

		// global descriptors
		DescriptorImages	m_imagesDescriptor;
		DescriptorSampler	m_samplerDescriptorTextures;
		Sampler				m_samplerTextures;

		// render passes
		RenderPass m_renderPassGame;
		RenderPass m_renderPassPostprocess;
		RenderPass m_renderPassImgui;

		CommandBuffer m_primaryCommandBuffers;		

		// frame buffers game 
		FrameBuffer m_gameFrameBuffers;
		Image		m_gameDepthImage;
		ImageView	m_gameDepthImageView;
		Sampler		m_gameColorSampler;
		Image		m_gameColorImage;
		ImageView	m_gameColorImageView;

		// frame buffers postprocess 
		FrameBuffer m_ppFramebuffers;
		Sampler		m_ppColorSampler;
		Image		m_ppColorImage;
		ImageView	m_ppColorImageView;

		// frame buffers swapchain 
		FrameBuffer m_swapchainFramebuffers;



		void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler* _samplerDescriptor, VkPipelineLayout _pipelineLayout );

		void RecordCommandBufferForward( const size_t _index );
		void RecordCommandBufferPostProcess( const size_t _index );
		void RecordCommandBufferImgui( const size_t _index );
		void RecordCommandBufferUI( const size_t _index );
		void RecordPrimaryCommandBuffer( const size_t _index );
		void RecordCommandBufferDebug( const size_t _index );
		void RecordAllCommandBuffers();

		void CreateCommandBuffers();
		void CreateFramebuffers( const VkExtent2D _extent );
		bool CreateRenderPasses();
		bool CreateTextureDescriptor();
		void CreateShaders();
		void DestroyShaders();
		void CreatePipelines();
		void DestroyPipelines();

		void UpdateUniformBuffers( Device& _device, const size_t _index );
		bool SubmitCommandBuffers();
	};
}
