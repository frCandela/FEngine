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

namespace fan
{
	class Window;
	class Mesh;
	class UIMesh;
	class Instance;
	class Device;
	class SwapChain;
	class ImguiPipeline;
	class FrameBuffer;
	class Buffer;
	class Sampler;
	class DescriptorTextures;
	class DescriptorSampler;
	class PostprocessPipeline;
	class ForwardPipeline;
	class UIPipeline;
	class RenderPass;
	class DebugPipeline;
	

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
	struct DrawData
	{
		Mesh* mesh;
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

		PostprocessPipeline*	GetPostprocessPipeline() { return m_postprocessPipeline; }
		ForwardPipeline*		GetForwardPipeline() { return m_forwardPipeline; }
		glm::vec4				GetClearColor()					const { return m_clearColor; }
		const FrameBuffer*		GetGameFrameBuffers()			const { return m_gameFrameBuffers; }
		const FrameBuffer*		GetPostProcessFramebuffers()	const { return m_postProcessFramebuffers; }
		const FrameBuffer*		GetSwapchainFramebuffers()		const { return m_swapchainFramebuffers; }
		float					GetWindowAspectRatio() const;

		void SetClearColor( glm::vec4 _color ) { m_clearColor = _color; }
		void SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position );
		void SetDirectionalLights( const std::vector<DrawDirectionalLight>& _lightData );
		void SetPointLights( const std::vector<DrawPointLight>& _lightData );
		void SetDrawData( const std::vector<DrawMesh>& _drawData );
		void SetUIDrawData( const std::vector<DrawUIMesh>& _drawData );
		void SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles );

		const std::vector< DrawData >& GetDrawData() const { return m_meshDrawArray; }
	private:

		VkExtent2D m_gameExtent = { 1,1 };

		std::vector< DrawData >		m_meshDrawArray;
		std::vector< UIDrawData >	m_uiMeshDrawArray;

		Window& m_window;

		// pipelines
		ImguiPipeline*			m_imguiPipeline;
		PostprocessPipeline*	m_postprocessPipeline;
		ForwardPipeline*		m_forwardPipeline;
		UIPipeline*				m_uiPipeline;
		DebugPipeline*			m_debugLinesPipeline;
		DebugPipeline*			m_debugLinesPipelineNoDepthTest;
		DebugPipeline*			m_debugTrianglesPipeline;

		// global descriptors
		DescriptorTextures* m_imagesDescriptor = nullptr;
		DescriptorSampler*	m_samplerDescriptorTextures = nullptr;
		Sampler*			m_samplerTextures = nullptr;
		DescriptorSampler*	m_samplerDescriptorUI = nullptr;
		Sampler*			m_samplerUI = nullptr;

		// render passes
		RenderPass m_renderPassGame;
		RenderPass m_renderPassPostprocess;
		RenderPass m_renderPassImgui;

		// command buffers
		CommandBuffer m_primaryCommandBuffers;
		CommandBuffer m_geometryCommandBuffers;
		CommandBuffer m_imguiCommandBuffers;
		CommandBuffer m_uiCommandBuffers;
		CommandBuffer m_postprocessCommandBuffers;
		CommandBuffer m_debugCommandBuffers;

		// frame buffers
		FrameBuffer* m_gameFrameBuffers = VK_NULL_HANDLE;
		FrameBuffer* m_postProcessFramebuffers = VK_NULL_HANDLE;
		FrameBuffer* m_swapchainFramebuffers = VK_NULL_HANDLE;

		// data
		Buffer*		m_quadVertexBuffer;
		glm::vec4	m_clearColor;

		// debug data
		bool m_hasNoDebugToDraw = true;
		std::vector<Buffer*>		m_debugLinesvertexBuffers;
		std::vector<Buffer*>		m_debugLinesNoDepthTestVertexBuffers;
		std::vector<Buffer*>		m_debugTrianglesvertexBuffers;
		int m_numDebugLines = 0;
		int m_numDebugLinesNoDepthTest = 0;
		int m_numDebugTriangle = 0;	

		void RecordCommandBufferPostProcess( const size_t _index );
		void RecordCommandBufferImgui( const size_t _index );
		void RecordCommandBufferUI( const size_t _index );
		void RecordCommandBufferGeometry( const size_t _index );
		void RecordPrimaryCommandBuffer( const size_t _index );
		void RecordCommandBufferDebug( const size_t _index );
		void RecordAllCommandBuffers();

		void CreateQuadVertexBuffer();
		bool CreateCommandBuffers();
		void CreateFramebuffers();
		bool CreateRenderPasses();
		bool CreateTextureDescriptor();
		
		void UpdateUniformBuffers( const size_t _index );
		bool SubmitCommandBuffers();
		void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler* _samplerDescriptor, VkPipelineLayout _pipelineLayout );
	};
}
