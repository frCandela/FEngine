#pragma once

#include "vulkan/util/vkVertex.h"

namespace scene {
	class Camera;
	class Mesh;
}

namespace vk {

	class Instance;
	class Window;
	class Device;
	class SwapChain;
	class ImguiPipeline;
	class FrameBuffer;
	class Image;
	class ImageView;
	class Shader;
	class Buffer;
	class Sampler;
	class PostprocessPipeline;
	class ForwardPipeline;
	class DebugPipeline;
	struct MeshData;

	class Renderer {
	public:
		Renderer(const VkExtent2D _size);
		~Renderer();

		bool WindowIsOpen();
		void DrawFrame();

		static Renderer & GetRenderer() {	return * ms_globalRenderer; }

		void ReloadShaders();
		void UpdateDebugBuffer(const int _index);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		ImguiPipeline *			GetImguiPipeline()			{ return m_imguiPipeline; }
		PostprocessPipeline *	GetPostprocessPipeline()	{ return m_postprocessPipeline; }
		ForwardPipeline *		GetForwardPipeline()		{ return m_forwardPipeline; }
		glm::vec4				GetClearColor() const		{ return m_clearColor;  }

		void ClearDebug() { m_vertices.clear(); }
		void SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
		void SetMainCamera( scene::Camera * _camera) { m_mainCamera = _camera; }

		bool HasNoDebugToDraw() const { return m_vertices.empty(); }

		void AddMesh(scene::Mesh * _mesh);
		void RemoveMesh(scene::Mesh * _mesh);
		void DebugLine(glm::vec3 start, glm::vec3 end, glm::vec4 color);
	private:
		//SCENE REFERENCES
		scene::Camera * m_mainCamera;
		std::vector <MeshData> m_meshList;

		// DEBUG DATA
		std::vector<DebugVertex> m_vertices;
		std::vector<Buffer *> m_vertexBuffers;

		// VULKAN OBJECTS
		Instance *		m_instance;
		Window *		m_window;
		Device &		m_device;
		SwapChain  *	m_swapchain;

		ImguiPipeline *			m_imguiPipeline;
		PostprocessPipeline *	m_postprocessPipeline;
		ForwardPipeline *		m_forwardPipeline;
		DebugPipeline *			m_debugPipeline;

		VkRenderPass	m_renderPass;
		VkRenderPass	m_renderPassPostprocess;
		VkRenderPass	m_renderPassUI;

		VkCommandPool	m_commandPool;
		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;
		std::vector<VkCommandBuffer> m_debugCommandBuffers;
		std::vector<VkCommandBuffer> m_postprocessCommandBuffers;

		std::vector<bool> m_reloadGeometryCommandBuffers;

		std::vector< FrameBuffer * > m_forwardFrameBuffers;
		std::vector< FrameBuffer * > m_swapchainFramebuffers;

		glm::vec4 m_clearColor;

		static Renderer * ms_globalRenderer;
		
		bool ResetCommandPool();
		void UpdateUniformBuffer();		
		bool SubmitCommandBuffers();

		void RecordCommandBufferPostProcess	( const int _index);
		void RecordCommandBufferImgui		( const int _index);
		void RecordCommandBufferDebug		(const int _index);
		void RecordCommandBufferGeometry	( const int _index);		
		void RecordPrimaryCommandBuffer		( const int _index);
		void RecordAllCommandBuffers();

		bool CreateCommandBuffers();
		bool CreateCommandPool();
		void CreateForwardFramebuffers();
		void CreateSwapchainFramebuffers();

		bool CreateRenderPass();
		bool CreateRenderPassPostprocess();
		bool CreateRenderPassUI();

		void DeleteCommandPool();
		void DeleteRenderPass();
		void DeleteRenderPassPostprocess();
		void DeleteForwardFramebuffers();
		void DeleteSwapchainFramebuffers();
	};
}