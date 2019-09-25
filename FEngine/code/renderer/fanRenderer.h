#pragma once

#include "renderer/fanUniforms.h"
#include "renderer/util/fanVertex.h"
#include "core/math/shapes/fanAABB.h"
#include "core/fanSingleton.h"

namespace fan
{
	class Model;
	class Transform;
	class Material;
	class PointLight;

	class Window;
	class Mesh;
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
		Model *		model = nullptr;
		Transform *	transform = nullptr;
		Material *	material = nullptr;
	};

	//================================================================================================================================
	// TODO Renderer should not be a singleton
	//================================================================================================================================
	class Renderer : public Singleton<Renderer> {
	private:
		struct Camera {
			glm::vec3 position;
			glm::mat4 projection;
			glm::mat4 view;
		};

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
		ImguiPipeline *			GetImguiPipeline() { return m_imguiPipeline; }
		PostprocessPipeline *	GetPostprocessPipeline() { return m_postprocessPipeline; }
		ForwardPipeline *		GetForwardPipeline() { return m_forwardPipeline; }
		glm::vec4				GetClearColor() const { return m_clearColor; }
		RessourceManager *		GetRessourceManager() const { return m_ressourceManager; }

		void  SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
		void  SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position );
		void  SetDirectionalLight( const int _index, const glm::vec4 _direction, const glm::vec4 _ambiant, const glm::vec4 _diffuse, const glm::vec4 _specular );
		void  SetNumDirectionalLights( const uint32_t _num );

		float GetWindowAspectRatio() const;
		bool  HasNoDebugToDraw() const { return m_debugLinesNoDepthTest.empty() && m_debugLines.empty() && m_debugTriangles.empty(); }

		const std::vector < DrawData > & GetDrawData() const { return m_drawData; }

		void RegisterMaterial			( Material *		 _material );
		void UnRegisterMaterial			( Material *		 _material );
		void RegisterModel				( Model *			 _model );
		void UnRegisterModel			( Model *			 _model );


		//LightsUniforms	GetLightUniforms() const { return m_pointLightUniform; }
		//void			SetLightUniforms( const LightsUniforms& _lights );

		void Clear();

		void					DebugPoint	  ( const btVector3 _pos, const Color _color);
		void					DebugLine	  ( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable = true);
		void					DebugTriangle ( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color);
		std::vector< btVector3> DebugCube	  ( const btTransform _transform, const float _halfSize, const Color _color);
		std::vector< btVector3> DebugSphere	  ( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color);
		std::vector< btVector3> DebugCone	  ( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color);
		void					DebugAABB	  ( const AABB & _aabb, const Color _color);

	private:
		Camera m_camera;
		LightsUniforms m_lightsUniform;

		std::vector < DrawData >			 m_drawData;
		RessourceManager *  m_ressourceManager;
		bool m_mustUpdateDynamicUniformsFrag = false;

		// DEBUG DATA
		std::vector<DebugVertex>	m_debugLines;
		std::vector<Buffer *>		m_debugLinesvertexBuffers;
		std::vector<DebugVertex>	m_debugLinesNoDepthTest;
		std::vector<Buffer *>		m_debugLinesNoDepthTestVertexBuffers;
		std::vector<DebugVertex>	m_debugTriangles;
		std::vector<Buffer *>		m_debugTrianglesvertexBuffers;

		// VULKAN OBJECTS
		Instance *		m_instance;
		Window *		m_window;
		Device *		m_device;
		SwapChain  *	m_swapchain;

		ImguiPipeline *			m_imguiPipeline;
		PostprocessPipeline *	m_postprocessPipeline;
		ForwardPipeline *		m_forwardPipeline;
		DebugPipeline *			m_debugLinesPipeline;
		DebugPipeline *			m_debugLinesPipelineNoDepthTest;
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

		bool ResetCommandPool();
		void UpdateSceneUniforms();
		void UpdateUniformBuffer();
		bool SubmitCommandBuffers();

		void ClearDebug() {
			m_debugLines.clear();
			m_debugLinesNoDepthTest.clear();
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
