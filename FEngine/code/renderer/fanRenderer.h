#pragma once

#include "renderer/util/fanVertex.h"
#include "core/math/shapes/fanAABB.h"
#include "core/fanSingleton.h"


namespace fan
{
	class Mesh;
	class Window;

	namespace scene {
		class Camera;
		class Model;
		class Transform;
		class Material;
	}

	namespace vk {
		class Instance;
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
		class RessourceManager;

		struct MeshData;

		struct DrawData {
			MeshData *			meshData = nullptr;
			scene::Model *		model = nullptr;
			scene::Transform *	transform = nullptr;
			scene::Material *	material = nullptr;
		};
	}
	   	 
	//================================================================================================================================
	//================================================================================================================================
	class Renderer : public Singleton<Renderer>{
	public:
		void Initialize(const VkExtent2D _size, const glm::ivec2 _position);
		void Destroy();

		bool WindowIsOpen();
		void DrawFrame();
		void WaitIdle();

		void ReloadShaders();
		void UpdateDebugBuffer(const int _index);

		VkCommandBuffer BeginSingleTimeCommands();
		void			EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		Window *				GetWindow() { return m_window; }
		vk::ImguiPipeline *			GetImguiPipeline() { return m_imguiPipeline; }
		vk::PostprocessPipeline *	GetPostprocessPipeline() { return m_postprocessPipeline; }
		vk::ForwardPipeline *		GetForwardPipeline() { return m_forwardPipeline; }
		glm::vec4				GetClearColor() const { return m_clearColor; }
		vk::RessourceManager *	GetRessourceManager() const { return m_ressourceManager; }

		void SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
		void SetMainCamera(scene::Camera * _camera);

		bool HasNoDebugToDraw() const { return m_debugLines.empty() && m_debugTriangles.empty(); }

		const std::vector < vk::DrawData > & GetDrawData() const { return m_drawData; }

		void RegisterMaterial(scene::Material * _model);
		void UnRegisterMaterial(scene::Material * _model);
		void RegisterModel(scene::Model * _model);
		void UnRegisterModel(scene::Model * _model);

		void					DebugPoint(const btVector3 _pos, const Color _color);
		void					DebugLine(const btVector3 _start, const btVector3 _end, const Color _color);
		void					DebugTriangle(const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color);
		std::vector< btVector3> DebugCube(const btTransform _transform, const float _halfSize, const Color _color);
		std::vector< btVector3> DebugSphere(const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color);
		std::vector< btVector3> DebugCone(const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color);
		void					DebugAABB(const shape::AABB & _aabb, const Color _color);

	private:
		//SCENE REFERENCES
		scene::Camera * m_mainCamera;
		scene::Transform * m_mainCameraTransform;


		std::vector < vk::DrawData > m_drawData;
		vk::RessourceManager *  m_ressourceManager;

		// DEBUG DATA
		std::vector<vk::DebugVertex> m_debugLines;
		std::vector<vk::Buffer *> m_debugLinesvertexBuffers;
		std::vector<vk::DebugVertex> m_debugTriangles;
		std::vector<vk::Buffer *> m_debugTrianglesvertexBuffers;

		// VULKAN OBJECTS
		vk::Instance *		m_instance;
		Window *		m_window;
		vk::Device *		m_device;
		vk::SwapChain  *	m_swapchain;

		vk::ImguiPipeline *			m_imguiPipeline;
		vk::PostprocessPipeline *	m_postprocessPipeline;
		vk::ForwardPipeline *		m_forwardPipeline;
		vk::DebugPipeline *			m_debugLinesPipeline;
		vk::DebugPipeline *			m_debugTrianglesPipeline;

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

		std::vector< vk::FrameBuffer * > m_forwardFrameBuffers;
		std::vector< vk::FrameBuffer * > m_swapchainFramebuffers;

		glm::vec4 m_clearColor;

		bool ResetCommandPool();
		void UpdateUniformBuffer(bool _forceFullRebuild = false);
		bool SubmitCommandBuffers();

		void ClearDebug() {
			m_debugLines.clear();
			m_debugTriangles.clear();
		}

		void RecordCommandBufferPostProcess(const int _index);
		void RecordCommandBufferImgui(const int _index);
		void RecordCommandBufferDebug(const int _index);
		void RecordCommandBufferGeometry(const int _index);
		void RecordPrimaryCommandBuffer(const int _index);
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
