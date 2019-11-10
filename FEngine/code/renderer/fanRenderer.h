#pragma once

#include "renderer/util/fanVertex.h"
#include "core/math/shapes/fanAABB.h"

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
	class Image;
	class ImageView;
	class Shader;
	class Buffer;
	class Sampler;
	class PostprocessPipeline;
	class ForwardPipeline;
	class DebugPipeline;
	class UIPipeline;
	class Color;
	class RessourceManager;

	// Used to set uniforms
	struct DrawMesh {
		Mesh * mesh;
		glm::mat4 modelMatrix;
		glm::mat4 normalMatrix;
		glm::vec4 color;
		uint32_t shininess;
		uint32_t textureIndex;
	};

	struct DrawUIMesh {
		UIMesh * mesh;
		glm::vec2 position;
		glm::vec2 scale;
	};

	// Used to batch rendering
	struct DrawData
	{
		Mesh * mesh;
		uint32_t textureIndex;
	};

	// Used to batch rendering
	struct UIDrawData
	{
		UIMesh * mesh;
		uint32_t textureIndex;
	};

	//================================================================================================================================
	//================================================================================================================================
	class Renderer {
	private:


	public:
		Renderer(const VkExtent2D _size, const glm::ivec2 _position);
		~Renderer();

		bool WindowIsOpen();
		void DrawFrame();
		void ClearDebug();
		void WaitIdle();

		void ReloadShaders();
		void UpdateDebugBuffer(const size_t _index);

		Window *				GetWindow() { return m_window; }
		ImguiPipeline *			GetImguiPipeline() { return m_imguiPipeline; }
		PostprocessPipeline *	GetPostprocessPipeline() { return m_postprocessPipeline; }
		ForwardPipeline *		GetForwardPipeline() { return m_forwardPipeline; }
		glm::vec4				GetClearColor() const { return m_clearColor; }
		RessourceManager *		GetRessourceManager() const { return m_ressourceManager; }

		void SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
		void SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position );
		void SetDirectionalLight( const int _index, const glm::vec4 _direction, const glm::vec4 _ambiant, const glm::vec4 _diffuse, const glm::vec4 _specular );
		void SetNumDirectionalLights( const uint32_t _num );
		void SetPointLight( const int _index, const glm::vec3 _position, const glm::vec3 _diffuse, const glm::vec3 _specular, const glm::vec3 _ambiant, const glm::vec3 _constantLinearQuadratic );
		void SetNumPointLights( const uint32_t _num );
		void SetDrawData( const std::vector<DrawMesh> & _drawData );
		void SetUIDrawData( const std::vector<DrawUIMesh> & _drawData );

		float GetWindowAspectRatio() const;
		bool  HasNoDebugToDraw() const { return m_debugLinesNoDepthTest.empty() && m_debugLines.empty() && m_debugTriangles.empty(); }
		void Clear() { m_meshDrawArray.clear(); }

		const std::vector< DrawData > & GetMeshArray() const { return m_meshDrawArray; }

		void					DebugPoint	  ( const btVector3 _pos, const Color _color);
		void					DebugLine	  ( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable = true);
		void					DebugTriangle ( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color);
		void					DebugTriangles( const std::vector<btVector3>& _triangles, const std::vector<Color>& _colors );
		void					DebugCircle	  ( const btVector3 _pos , const float _radius, btVector3 _axis, uint32_t _nbSegments, const Color _color );
		std::vector< btVector3> DebugCube	  ( const btTransform _transform, const btVector3 _halfExtent, const Color _color);
		std::vector< btVector3> DebugSphere	  ( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color);
		std::vector< btVector3> DebugCone	  ( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color);
		void					DebugAABB	  ( const AABB & _aabb, const Color _color);

	private:
		std::vector< DrawData > m_meshDrawArray;
		std::vector< UIDrawData > m_uiMeshDrawArray;

		RessourceManager *  m_ressourceManager;

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
		UIPipeline *			m_uiPipeline;
		DebugPipeline *			m_debugLinesPipeline;
		DebugPipeline *			m_debugLinesPipelineNoDepthTest;
		DebugPipeline *			m_debugTrianglesPipeline;

		VkRenderPass	m_renderPass;
		VkRenderPass	m_renderPassPostprocess;

		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;
		std::vector<VkCommandBuffer> m_uiCommandBuffers;
		std::vector<VkCommandBuffer> m_debugCommandBuffers;
		std::vector<VkCommandBuffer> m_postprocessCommandBuffers;


		FrameBuffer * m_forwardFrameBuffers;
		FrameBuffer * m_swapchainFramebuffers;

		Buffer * m_quadVertexBuffer;
		glm::vec4 m_clearColor;

		void UpdateUniformBuffers( const size_t _index );
		
		bool SubmitCommandBuffers();
		void CreateQuadVertexBuffer();

		void RecordCommandBufferPostProcess(const size_t _index);
		void RecordCommandBufferImgui(const size_t _index);
		void RecordCommandBufferUI(const size_t _index);
		void RecordCommandBufferDebug(const size_t _index);
		void RecordCommandBufferGeometry(const size_t _index);
		void RecordPrimaryCommandBuffer(const size_t _index);
		void RecordAllCommandBuffers();

		bool CreateCommandBuffers();
		void CreateForwardFramebuffers();
		void CreateSwapchainFramebuffers();

		bool CreateRenderPass();
		bool CreateRenderPassPostprocess();

		void DeleteRenderPass();
		void DeleteRenderPassPostprocess();
	};
}
