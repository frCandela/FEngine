#pragma once

#include "renderer/util/fanVertex.h"
#include "core/math/shapes/fanAABB.h"
#include "core/memory/fanAlignedMemory.h"

namespace fan
{
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

	//================================================================================================================================
	//================================================================================================================================
	class Renderer {
	public:
		Renderer(const VkExtent2D _size, const glm::ivec2 _position);
		~Renderer();

		bool WindowIsOpen();
		void DrawFrame();
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
		void SetDynamicUniformVert( const glm::mat4 _modelMat, const glm::mat4 _rotationMat, const uint32_t _index );
		void SetDynamicUniformFrag( const glm::vec3  _color, const glm::int32 _shininess, const glm::int32 _textureIndex, const uint32_t _index );
		void SetMeshAt( const uint32_t _index, Mesh * _mesh );
		void SetNumMesh( const uint32_t _num );
		void SetTransformAt( const uint32_t _index, glm::mat4 _modelMatrix,	glm::mat4 _normalMatrix );
		void SetMaterialAt( const uint32_t _index, const glm::vec3 _color, const uint32_t _shininess, const uint32_t _textureIndex );

		float GetWindowAspectRatio() const;
		bool  HasNoDebugToDraw() const { return m_debugLinesNoDepthTest.empty() && m_debugLines.empty() && m_debugTriangles.empty(); }
		void Clear() { m_numMesh = 0; }
		uint32_t GetNumMesh() const { return m_numMesh; }
		const std::array< Mesh *, GlobalValues::s_maximumNumModels > & GetMeshArray() const { return m_meshDrawArray; }

		void					DebugPoint	  ( const btVector3 _pos, const Color _color);
		void					DebugLine	  ( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable = true);
		void					DebugTriangle ( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color);
		std::vector< btVector3> DebugCube	  ( const btTransform _transform, const float _halfSize, const Color _color);
		std::vector< btVector3> DebugSphere	  ( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color);
		std::vector< btVector3> DebugCone	  ( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color);
		void					DebugAABB	  ( const AABB & _aabb, const Color _color);

	private:
		std::array< Mesh *, GlobalValues::s_maximumNumModels > m_meshDrawArray;
		uint32_t m_numMesh;

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
		DebugPipeline *			m_debugLinesPipeline;
		DebugPipeline *			m_debugLinesPipelineNoDepthTest;
		DebugPipeline *			m_debugTrianglesPipeline;

		VkRenderPass	m_renderPass;
		VkRenderPass	m_renderPassPostprocess;
		VkRenderPass	m_renderPassUI;


		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;
		std::vector<VkCommandBuffer> m_debugCommandBuffers;
		std::vector<VkCommandBuffer> m_postprocessCommandBuffers;

		FrameBuffer * m_forwardFrameBuffers;
		FrameBuffer * m_swapchainFramebuffers;

		Buffer * m_quadVertexBuffer;
		glm::vec4 m_clearColor;

		void UpdateUniformBuffers( const size_t _index );
		
		bool SubmitCommandBuffers();
		void CreateQuadVertexBuffer();
		void ClearDebug();

		void RecordCommandBufferPostProcess(const size_t _index);
		void RecordCommandBufferImgui(const size_t _index);
		void RecordCommandBufferDebug(const size_t _index);
		void RecordCommandBufferGeometry(const size_t _index);
		void RecordPrimaryCommandBuffer(const size_t _index);
		void RecordAllCommandBuffers();

		bool CreateCommandBuffers();
		void CreateForwardFramebuffers();
		void CreateSwapchainFramebuffers();

		bool CreateRenderPass();
		bool CreateRenderPassPostprocess();
		bool CreateRenderPassUI();

		void DeleteRenderPass();
		void DeleteRenderPassPostprocess();
	};
}
