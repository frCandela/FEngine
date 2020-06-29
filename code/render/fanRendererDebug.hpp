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
		bool HasNoDebugToDraw() const { return m_hasNoDebugToDraw; }
		void RecordCommandBufferDebug( const size_t _index );
		void ReloadShaders();
		void UpdateDebugBuffer( const std::vector<DebugVertex>&	_debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles );
		void Resize( const VkExtent2D _extent );
		void UpdateUniformBuffers( const size_t _index );
		void SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view );
		VkCommandBuffer* GetCommandBuffer( const size_t _index ) { return &m_debugCommandBuffers[ _index ]; };
		std::vector<VkCommandBuffer>& GetCommandBuffers() { return m_debugCommandBuffers; }
	private:
		Device& m_device;
		SwapChain& m_swapchain;
		VkRenderPass m_renderPass = VK_NULL_HANDLE;
		FrameBuffer* m_frameBuffer = nullptr;

		DebugPipeline* m_debugLinesPipeline;
		DebugPipeline* m_debugLinesPipelineNoDepthTest;
		DebugPipeline* m_debugTrianglesPipeline;

		std::vector<VkCommandBuffer> m_debugCommandBuffers;

		// vertex buffers
		bool m_hasNoDebugToDraw = true;
		std::vector<Buffer*>		m_debugLinesvertexBuffers;
		std::vector<Buffer*>		m_debugLinesNoDepthTestVertexBuffers;
		std::vector<Buffer*>		m_debugTrianglesvertexBuffers;
		int m_numDebugLines = 0;
		int m_numDebugLinesNoDepthTest = 0;
		int m_numDebugTriangle = 0;
	};
}
