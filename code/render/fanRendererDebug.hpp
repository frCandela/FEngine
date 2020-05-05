#pragma once

#include "core/math/shapes/fanAABB.hpp"
#include "render/util/fanVertex.hpp"
#include "bullet/LinearMath/btTransform.h"

namespace fan
{
	class Buffer;
	class Device;
	class SwapChain;
	class DebugPipeline;
	class FrameBuffer;

	//================================================================================================================================
	// Debug draw utility for the renderer class
	//================================================================================================================================
	class RendererDebug
	{
	public:
		RendererDebug( Device& _device, SwapChain& _swapchain );
		~RendererDebug();

		void Create( VkRenderPass _renderPass, FrameBuffer* _frameBuffer );
		void ClearDebug();
		bool HasNoDebugToDraw() const { return m_debugLinesNoDepthTest.empty() && m_debugLines.empty() && m_debugTriangles.empty(); }
		void RecordCommandBufferDebug( const size_t _index );
		void ReloadShaders();
		void UpdateDebugBuffer( const size_t _index );
		void Resize( const VkExtent2D _extent );
		void UpdateUniformBuffers( const size_t _index );
		void SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view );
		VkCommandBuffer* GetCommandBuffer( const size_t _index ) { return &m_debugCommandBuffers[ _index ]; };
		std::vector<VkCommandBuffer>& GetCommandBuffers() { return m_debugCommandBuffers; }

		void DebugPoint( const btVector3 _pos, const Color _color );
		void DebugLine( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable = true );
		void DebugTriangle( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color );
		void DebugTriangles( const std::vector<btVector3>& _triangles, const std::vector<Color>& _colors );
		void DebugCircle( const btVector3 _pos, const float _radius, btVector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable = true );
		void DebugCube( const btTransform _transform, const btVector3 _halfExtent, const Color _color, const bool _depthTestEnable = true );
		void DebugIcoSphere( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable = true );
		void DebugSphere( const btTransform _transform, const float _radius, const Color _color, const bool _depthTestEnable = true );
		void DebugCone( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color );
		void DebugAABB( const AABB& _aabb, const Color _color );

		static RendererDebug& Get() { return *s_rendererInstance; }
		static void Init( RendererDebug* const _rendererDebug ) { assert( s_rendererInstance == nullptr ); s_rendererInstance = _rendererDebug; }
	private:
		Device& m_device;
		SwapChain& m_swapchain;
		VkRenderPass m_renderPass = VK_NULL_HANDLE;
		FrameBuffer* m_frameBuffer = nullptr;

		static RendererDebug* s_rendererInstance; // Used for global debug draw

		DebugPipeline* m_debugLinesPipeline;
		DebugPipeline* m_debugLinesPipelineNoDepthTest;
		DebugPipeline* m_debugTrianglesPipeline;

		std::vector<VkCommandBuffer> m_debugCommandBuffers;

		// vertex data
		std::vector<DebugVertex>	m_debugLines;
		std::vector<DebugVertex>	m_debugLinesNoDepthTest;
		std::vector<DebugVertex>	m_debugTriangles;

		// vertex buffers
		std::vector<Buffer*>		m_debugLinesvertexBuffers;
		std::vector<Buffer*>		m_debugLinesNoDepthTestVertexBuffers;
		std::vector<Buffer*>		m_debugTrianglesvertexBuffers;
	};
}
