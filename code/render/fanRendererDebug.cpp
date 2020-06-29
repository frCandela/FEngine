#include "render/fanRendererDebug.hpp"

#include <array>
#include "core/fanDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RendererDebug::RendererDebug( Device& _device, SwapChain& _swapchain )
		: m_device( _device )
		, m_swapchain( _swapchain )
	{
		m_debugLinesvertexBuffers.resize( m_swapchain.GetSwapchainImagesCount() );
		m_debugLinesNoDepthTestVertexBuffers.resize( m_swapchain.GetSwapchainImagesCount() );
		m_debugTrianglesvertexBuffers.resize( m_swapchain.GetSwapchainImagesCount() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::Create( VkRenderPass _renderPass, FrameBuffer* _frameBuffer )
	{
		m_renderPass = _renderPass;
		m_frameBuffer = _frameBuffer;


		m_debugLinesPipeline = new DebugPipeline( m_device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true );
		m_debugLinesPipeline->Init( _renderPass, m_swapchain.GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipeline->CreateDescriptors( m_swapchain.GetSwapchainImagesCount() );
		m_debugLinesPipeline->Create();

		m_debugLinesPipelineNoDepthTest = new DebugPipeline( m_device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false );
		m_debugLinesPipelineNoDepthTest->Init( _renderPass, m_swapchain.GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipelineNoDepthTest->CreateDescriptors( m_swapchain.GetSwapchainImagesCount() );
		m_debugLinesPipelineNoDepthTest->Create();

		m_debugTrianglesPipeline = new DebugPipeline( m_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false );
		m_debugTrianglesPipeline->Init( _renderPass, m_swapchain.GetExtent(), "code/shaders/debugTriangles.vert", "code/shaders/debugTriangles.frag" );
		m_debugTrianglesPipeline->CreateDescriptors( m_swapchain.GetSwapchainImagesCount() );
		m_debugTrianglesPipeline->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	RendererDebug::~RendererDebug()
	{
		delete m_debugLinesPipeline;
		delete m_debugLinesPipelineNoDepthTest;
		delete m_debugTrianglesPipeline;

		for ( int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++ )
		{
			delete m_debugLinesvertexBuffers[ bufferIndex ];
		} m_debugLinesvertexBuffers.clear();

		for ( int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++ )
		{
			delete m_debugLinesNoDepthTestVertexBuffers[ bufferIndex ];
		} m_debugLinesNoDepthTestVertexBuffers.clear();

		for ( int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++ )
		{
			delete m_debugTrianglesvertexBuffers[ bufferIndex ];
		} m_debugTrianglesvertexBuffers.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::Resize( const VkExtent2D _extent )
	{
		m_debugLinesPipeline->Resize( _extent );
		m_debugLinesPipelineNoDepthTest->Resize( _extent );
		m_debugTrianglesPipeline->Resize( _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::UpdateUniformBuffers( const size_t _index )
	{
		m_debugLinesPipeline->UpdateUniformBuffers( _index );
		m_debugLinesPipelineNoDepthTest->UpdateUniformBuffers( _index );
		m_debugTrianglesPipeline->UpdateUniformBuffers( _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view )
	{
		std::array< DebugPipeline*, 3 > debugLinesPipelines = { m_debugLinesPipeline, m_debugLinesPipelineNoDepthTest, m_debugTrianglesPipeline };
		for ( int pipelingIndex = 0; pipelingIndex < debugLinesPipelines.size(); pipelingIndex++ )
		{
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.model = glm::mat4( 1.0 );
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.view = _view;
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.proj = _projection;
			debugLinesPipelines[ pipelingIndex ]->m_debugUniforms.color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::UpdateDebugBuffer( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		m_hasNoDebugToDraw = _debugLinesNoDepthTest.empty() && _debugLines.empty() && _debugTriangles.empty();
		m_numDebugLines = (int)_debugLines.size();
		m_numDebugLinesNoDepthTest = (int)_debugLinesNoDepthTest.size();
		m_numDebugTriangle = (int)_debugTriangles.size();

		const uint32_t currentFrame = m_swapchain.GetCurrentFrame();

		SCOPED_PROFILE( update_buffer );
		if( _debugLines.size() > 0 )
		{
			SCOPED_PROFILE( lines );
			delete m_debugLinesvertexBuffers[currentFrame];	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			m_debugLinesvertexBuffers[currentFrame] = new Buffer( m_device );
			m_debugLinesvertexBuffers[currentFrame]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer( m_device );
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _debugLines.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesvertexBuffers[currentFrame]->GetBuffer(), size );
				m_device.EndSingleTimeCommands( cmd );
			}
		}

		if ( _debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth );
			delete m_debugLinesNoDepthTestVertexBuffers[currentFrame];
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			m_debugLinesNoDepthTestVertexBuffers[currentFrame] = new Buffer( m_device );
			m_debugLinesNoDepthTestVertexBuffers[currentFrame]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if ( size > 0 )
			{
				Buffer stagingBuffer( m_device );
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesNoDepthTestVertexBuffers[currentFrame]->GetBuffer(), size );
				m_device.EndSingleTimeCommands( cmd );
			}
		}

		if ( _debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles );
			delete m_debugTrianglesvertexBuffers[currentFrame];
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			m_debugTrianglesvertexBuffers[currentFrame] = new Buffer( m_device );
			m_debugTrianglesvertexBuffers[currentFrame]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if ( size > 0 )
			{
				Buffer stagingBuffer( m_device );
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _debugTriangles.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugTrianglesvertexBuffers[currentFrame]->GetBuffer(), size );
				m_device.EndSingleTimeCommands( cmd );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::ReloadShaders()
	{
		m_debugLinesPipeline->ReloadShaders();
		m_debugLinesPipelineNoDepthTest->ReloadShaders();
		m_debugTrianglesPipeline->ReloadShaders();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RendererDebug::RecordCommandBufferDebug( const size_t _index )
	{
		SCOPED_PROFILE( debug );
		if( HasNoDebugToDraw() == false )
		{
			VkCommandBuffer commandBuffer = m_debugCommandBuffers[_index];

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = m_renderPass;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = m_frameBuffer->GetFrameBuffer( _index );
			commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
			//commandBufferInheritanceInfo.queryFlags				=;
			//commandBufferInheritanceInfo.pipelineStatistics		=;

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

			if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
			{
				VkDeviceSize offsets[] = { 0 };
				if( m_numDebugLines > 0 )
				{
					m_debugLinesPipeline->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesvertexBuffers[_index]->GetBuffer() };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLines ), 1, 0, 0 );
				}
				if( m_numDebugLinesNoDepthTest > 0 )
				{
					m_debugLinesPipelineNoDepthTest->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesNoDepthTestVertexBuffers[_index]->GetBuffer() };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLinesNoDepthTest ), 1, 0, 0 );
				}
				if( m_numDebugTriangle > 0 )
				{
					m_debugTrianglesPipeline->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugTrianglesvertexBuffers[_index]->GetBuffer() };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugTriangle ), 1, 0, 0 );
				}
				if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
				{
					Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
				}
			}
			else
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
	}
}