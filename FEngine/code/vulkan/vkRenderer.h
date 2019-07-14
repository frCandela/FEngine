#pragma once

#include "vulkan/util/vkVertex.h"

namespace scene {
	class Camera;
	class Mesh;
	class Transform;
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
	class Color;

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


		void SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
		void SetMainCamera(scene::Camera * _camera);

		bool HasNoDebugToDraw() const { return m_debugLines.empty() && m_debugTriangles.empty(); }

		void AddMesh		( scene::Mesh * _mesh);
		void RemoveMesh		( scene::Mesh * _mesh);

		void					DebugPoint		( const btVector3 _pos, const vk::Color _color);
		void					DebugLine		( const btVector3 _start, const btVector3 _end, const vk::Color _color);
		void					DebugTriangle	( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const vk::Color _color);		
		std::vector< btVector3> DebugCube		( const btTransform _transform, const float _halfSize,	const vk::Color _color);
		std::vector< btVector3> DebugSphere		( const btTransform _transform,  const float _radius, const int _numSubdivisions, const vk::Color _color);
		std::vector< btVector3> DebugCone		( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const vk::Color _color);

	private:
		//SCENE REFERENCES
		scene::Camera * m_mainCamera;
		scene::Transform * m_mainCameraTransform;
		std::vector <MeshData> m_meshList;

		// DEBUG DATA
		std::vector<DebugVertex> m_debugLines;
		std::vector<Buffer *> m_debugLinesvertexBuffers;
		std::vector<DebugVertex> m_debugTriangles;
		std::vector<Buffer *> m_debugTrianglesvertexBuffers;

		// VULKAN OBJECTS
		Instance *		m_instance;
		Window *		m_window;
		Device &		m_device;
		SwapChain  *	m_swapchain;

		ImguiPipeline *			m_imguiPipeline;
		PostprocessPipeline *	m_postprocessPipeline;
		ForwardPipeline *		m_forwardPipeline;
		DebugPipeline *			m_debugLinesPipeline;
		DebugPipeline *			m_debugTrianglesPipeline;

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

		void ClearDebug() {
			m_debugLines.clear();
			m_debugTriangles.clear();
		}

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