#include "render/fanRenderer.hpp"

#include "core/fanDebug.hpp"
#include "core/input/fanInput.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/fanMeshManager.hpp"
#include "render/fanUIMesh.hpp"
#include "render/fanMesh.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanSwapChain.hpp"
#include "render/core/fanInstance.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/pipelines/fanPostprocessPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/pipelines/fanImguiPipeline.hpp"
#include "render/pipelines/fanUIPipeline.hpp"
#include "render/fanVertex.hpp"
#include "render/fanWindow.hpp"
#include "render/descriptors/fanDescriptorTexture.hpp"
#include "render/descriptors/fanDescriptorSampler.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer( Window& _window ) : m_window( _window )
	{
		m_debugLinesvertexBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_debugLinesNoDepthTestVertexBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_debugTrianglesvertexBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );

		m_clearColor = glm::vec4( 0.f, 0.f, 0.2f, 1.f );

		Mesh::s_resourceManager.Init( m_window.GetDevice() );
		Texture::s_resourceManager.Init( m_window.GetDevice() );
		UIMesh::s_resourceManager.Init( m_window.GetDevice() );

		CreateRenderPasses();
		CreateQuadVertexBuffer();
		CreateFramebuffers();

		m_samplerTextures.Create( m_window.GetDevice(), 0, 8, VK_FILTER_LINEAR );
		m_samplerDescriptorTextures = new DescriptorSampler( m_window.GetDevice(), m_samplerTextures.sampler );
		CreateTextureDescriptor();

		m_debugLinesPipeline = new DebugPipeline( m_window.GetDevice(), VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true );
		m_debugLinesPipeline->Init( m_renderPassGame.renderPass, m_window.GetSwapChain().GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_debugLinesPipeline->Create();

		m_debugLinesPipelineNoDepthTest = new DebugPipeline( m_window.GetDevice(), VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false );
		m_debugLinesPipelineNoDepthTest->Init( m_renderPassGame.renderPass, m_window.GetSwapChain().GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipelineNoDepthTest->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_debugLinesPipelineNoDepthTest->Create();

		m_debugTrianglesPipeline = new DebugPipeline( m_window.GetDevice(), VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false );
		m_debugTrianglesPipeline->Init( m_renderPassGame.renderPass, m_window.GetSwapChain().GetExtent(), "code/shaders/debugTriangles.vert", "code/shaders/debugTriangles.frag" );
		m_debugTrianglesPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_debugTrianglesPipeline->Create();

		m_forwardPipeline = new ForwardPipeline( m_window.GetDevice(), m_imagesDescriptor, m_samplerDescriptorTextures );
		m_forwardPipeline->Init( m_renderPassGame.renderPass, m_window.GetSwapChain().GetExtent(), "code/shaders/forward.vert", "code/shaders/forward.frag" );
		m_forwardPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_forwardPipeline->Create();

		m_samplerUI.Create( m_window.GetDevice(), 0, 1, VK_FILTER_NEAREST );
		m_samplerDescriptorUI = new DescriptorSampler( m_window.GetDevice(), m_samplerUI.sampler );
		m_uiPipeline = new UIPipeline( m_window.GetDevice(), m_imagesDescriptor, m_samplerDescriptorUI );
		m_uiPipeline->Init( m_renderPassPostprocess.renderPass, m_window.GetSwapChain().GetExtent(), "code/shaders/ui.vert", "code/shaders/ui.frag" );
		m_uiPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_uiPipeline->Create();

		m_postprocessPipeline = new PostprocessPipeline( m_window.GetDevice() );
		m_postprocessPipeline->SetGameImageView( m_gameFrameBuffers->GetColorAttachmentImageView() );
		m_postprocessPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_postprocessPipeline->Init( m_renderPassPostprocess.renderPass, m_window.GetSwapChain().GetExtent(), "code/shaders/postprocess.vert", "code/shaders/postprocess.frag" );
		m_postprocessPipeline->Create();

		m_imguiPipeline = new ImguiPipeline( m_window.GetDevice(), m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_imguiPipeline->SetGameView( m_postProcessFramebuffers->GetColorAttachmentImageView() );
		m_imguiPipeline->Create( m_renderPassImgui.renderPass, m_window.GetWindow(), m_window.GetSwapChain().GetExtent() );

		CreateCommandBuffers();
		RecordAllCommandBuffers();

		const size_t initialSize = 256;
		m_meshDrawArray.reserve( initialSize );
		m_forwardPipeline->m_dynamicUniformsVert.Resize( initialSize );
		m_forwardPipeline->m_dynamicUniformsMaterial.Resize( initialSize );
	}

	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer()
	{
		Device& device = m_window.GetDevice();

		vkDeviceWaitIdle( device.vkDevice );

		delete m_imguiPipeline;
		delete m_forwardPipeline;
		delete m_uiPipeline;

		Mesh::s_resourceManager.Clear();
		Texture::s_resourceManager.Clear();
		UIMesh::s_resourceManager.Clear();

		delete m_samplerDescriptorTextures;
		delete m_samplerDescriptorUI;
		delete m_imagesDescriptor;

		m_samplerTextures.Destroy( device );
		m_samplerUI.Destroy( device );
		m_quadVertexBuffer.Destroy( device );

		delete m_debugLinesPipeline;
		delete m_debugLinesPipelineNoDepthTest;
		delete m_debugTrianglesPipeline;

		for( int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++ )
		{
			m_debugLinesvertexBuffers[bufferIndex].Destroy( device );
		} m_debugLinesvertexBuffers.clear();

		for( int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++ )
		{
			m_debugLinesNoDepthTestVertexBuffers[bufferIndex].Destroy( device );;
		} m_debugLinesNoDepthTestVertexBuffers.clear();

		for( int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++ )
		{
			m_debugTrianglesvertexBuffers[bufferIndex].Destroy( device );;
		} m_debugTrianglesvertexBuffers.clear();

		delete m_gameFrameBuffers;
		delete m_postProcessFramebuffers;

		m_renderPassGame.Destroy( device.vkDevice );
		m_renderPassPostprocess.Destroy( device.vkDevice );
		m_renderPassImgui.Destroy( device.vkDevice );

		delete m_swapchainFramebuffers;

		delete m_postprocessPipeline;
	}



	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame()
	{
		SCOPED_PROFILE( draw_frame );

		const VkResult result = m_window.GetSwapChain().AcquireNextImage();
		if ( result == VK_ERROR_OUT_OF_DATE_KHR )
		{

			// window minimized
			if ( m_window.GetExtent().width == 0 && m_window.GetExtent().height == 0 )
			{
				glfwPollEvents();
				return;
			}
			else
			{
				ResizeSwapchain();
				return;
			}
		}
		else if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not acquire next image" );
		}
		else
		{
			vkWaitForFences( m_window.GetDevice().vkDevice, 1, m_window.GetSwapChain().GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max() );
			vkResetFences( m_window.GetDevice().vkDevice, 1, m_window.GetSwapChain().GetCurrentInFlightFence() );
		}

		ImGui::GetIO().DisplaySize = ImVec2( static_cast< float >( m_window.GetSwapChain().GetExtent().width ), static_cast< float >( m_window.GetSwapChain().GetExtent().height ) );

		if ( Texture::s_resourceManager.IsModified() )
		{
			WaitIdle();
			Debug::Log( "reload textures" );
			CreateTextureDescriptor();
			Texture::s_resourceManager.SetUnmodified();
		}

		const uint32_t currentFrame = m_window.GetSwapChain().GetCurrentFrame();
		UpdateUniformBuffers( currentFrame );
		{
			SCOPED_PROFILE( record_cmd );
			RecordCommandBufferGeometry( currentFrame );
			RecordCommandBufferDebug( currentFrame );
			RecordCommandBufferUI( currentFrame );
			RecordCommandBufferImgui( currentFrame );
			RecordPrimaryCommandBuffer( currentFrame );
		}

		{
			SCOPED_PROFILE( submit );
			SubmitCommandBuffers();
			m_window.GetSwapChain().PresentImage();
			m_window.GetSwapChain().StartNextFrame();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeGame( btVector2 _newSize )
	{
		WaitIdle();
		const VkExtent2D extent = { ( uint32_t ) _newSize[ 0 ], ( uint32_t ) _newSize[ 1 ] };
		m_gameExtent = extent;

		m_gameFrameBuffers->Resize( extent );
		m_postProcessFramebuffers->Resize( extent );

		m_postprocessPipeline->Resize( extent );
		m_forwardPipeline->Resize( extent );
		m_debugLinesPipeline->Resize( extent );
		m_debugLinesPipelineNoDepthTest->Resize( extent );
		m_debugTrianglesPipeline->Resize( extent );
		m_uiPipeline->Resize( extent );

		m_imguiPipeline->UpdateGameImageDescriptor();

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeSwapchain()
	{
		WaitIdle();
		const VkExtent2D extent = m_window.GetExtent();
		Debug::Get() << Debug::Severity::highlight << "Resize renderer: " << extent.width << "x" << extent.height << Debug::Endl();
		m_window.GetSwapChain().Resize( extent );
		m_swapchainFramebuffers->Resize( extent );


		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffers( const size_t _index )
	{
		m_postprocessPipeline->UpdateUniformBuffers( _index );
		m_forwardPipeline->UpdateUniformBuffers( _index );
		m_debugLinesPipeline->UpdateUniformBuffers( _index );
		m_debugLinesPipelineNoDepthTest->UpdateUniformBuffers( _index );
		m_debugTrianglesPipeline->UpdateUniformBuffers( _index );
		m_uiPipeline->UpdateUniformBuffers( _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::WaitIdle()
	{
		vkDeviceWaitIdle( m_window.GetDevice().vkDevice );
		Debug::Log( "Renderer idle" );
	}

	//================================================================================================================================
	//================================================================================================================================
	float  Renderer::GetWindowAspectRatio() const
	{
		return static_cast< float >( m_window.GetSwapChain().GetExtent().width ) / m_window.GetSwapChain().GetExtent().height;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position )
	{
		m_forwardPipeline->m_vertUniforms.view = _view;
		m_forwardPipeline->m_vertUniforms.proj = _projection;
		m_forwardPipeline->m_vertUniforms.proj[ 1 ][ 1 ] *= -1;

		m_forwardPipeline->m_fragUniforms.cameraPosition = _position;

		std::array< DebugPipeline*, 3 > debugLinesPipelines = { m_debugLinesPipeline, m_debugLinesPipelineNoDepthTest, m_debugTrianglesPipeline };
		for( int pipelingIndex = 0; pipelingIndex < debugLinesPipelines.size(); pipelingIndex++ )
		{
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.model = glm::mat4( 1.0 );
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.view = _view;
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.proj = m_forwardPipeline->m_vertUniforms.proj;
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetDirectionalLights( const std::vector<DrawDirectionalLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumDirectionalLight );
		m_forwardPipeline->m_lightUniforms.dirLightsNum = (uint32_t)_lightData.size();
		for( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawDirectionalLight& light = _lightData[i];
			m_forwardPipeline->m_lightUniforms.dirLights[i].direction = light.direction;
			m_forwardPipeline->m_lightUniforms.dirLights[i].ambiant = light.ambiant;
			m_forwardPipeline->m_lightUniforms.dirLights[i].diffuse = light.diffuse;
			m_forwardPipeline->m_lightUniforms.dirLights[i].specular = light.specular;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetPointLights( const std::vector<DrawPointLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumPointLights );
		m_forwardPipeline->m_lightUniforms.pointLightNum = (uint32_t)_lightData.size();
		for ( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawPointLight& light = _lightData[i];
			m_forwardPipeline->m_lightUniforms.pointlights[i].position	= light.position;
			m_forwardPipeline->m_lightUniforms.pointlights[i].diffuse	= light.diffuse;
			m_forwardPipeline->m_lightUniforms.pointlights[i].specular	= light.specular;
			m_forwardPipeline->m_lightUniforms.pointlights[i].ambiant	= light.ambiant;
			m_forwardPipeline->m_lightUniforms.pointlights[i].constant	= light.constant;
			m_forwardPipeline->m_lightUniforms.pointlights[i].linear	= light.linear;
			m_forwardPipeline->m_lightUniforms.pointlights[i].quadratic = light.quadratic;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDrawData( const std::vector<DrawMesh>& _drawData )
	{
		if ( _drawData.size() > m_meshDrawArray.capacity() )
		{
			Debug::Warning( "Resizing draw data arrays" );
			WaitIdle();
			const size_t newSize = 2 * _drawData.size();
			m_meshDrawArray.reserve( newSize );
			m_forwardPipeline->ResizeDynamicDescriptors( newSize );
		}

		m_meshDrawArray.clear();
		for ( int dataIndex = 0; dataIndex < _drawData.size(); dataIndex++ )
		{
			const DrawMesh& data = _drawData[ dataIndex ];

			if ( data.mesh != nullptr && !data.mesh->GetIndices().empty() )
			{
				// Transform
				m_forwardPipeline->m_dynamicUniformsVert[ dataIndex ].modelMat = data.modelMatrix;
				m_forwardPipeline->m_dynamicUniformsVert[ dataIndex ].normalMat = data.normalMatrix;

				// material
				m_forwardPipeline->m_dynamicUniformsMaterial[ dataIndex ].color = data.color;
				m_forwardPipeline->m_dynamicUniformsMaterial[ dataIndex ].shininess = data.shininess;

				// Mesh
				m_meshDrawArray.push_back( { data.mesh, data.textureIndex } );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetUIDrawData( const std::vector<DrawUIMesh>& _drawData )
	{
		m_uiMeshDrawArray.resize( _drawData.size() );
		for ( int meshIndex = 0; meshIndex < _drawData.size(); meshIndex++ )
		{
			const DrawUIMesh& uiData = _drawData[ meshIndex ];

			m_uiMeshDrawArray[ meshIndex ].mesh = uiData.mesh;
			m_uiMeshDrawArray[ meshIndex ].textureIndex = uiData.textureIndex;
			m_uiPipeline->m_dynamicUniformsVert[ meshIndex ].position = uiData.position;
			m_uiPipeline->m_dynamicUniformsVert[ meshIndex ].scale = uiData.scale;
			m_uiPipeline->m_dynamicUniformsVert[ meshIndex ].color = uiData.color;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		Device& device = m_window.GetDevice();

		m_hasNoDebugToDraw = _debugLinesNoDepthTest.empty() && _debugLines.empty() && _debugTriangles.empty();
		m_numDebugLines = (int)_debugLines.size();
		m_numDebugLinesNoDepthTest = (int)_debugLinesNoDepthTest.size();
		m_numDebugTriangle = (int)_debugTriangles.size();

		const uint32_t currentFrame = m_window.GetSwapChain().GetCurrentFrame();

		SCOPED_PROFILE( update_buffer );
		if( _debugLines.size() > 0 )
		{
			SCOPED_PROFILE( lines );
			m_debugLinesvertexBuffers[currentFrame].Destroy( device );	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			m_debugLinesvertexBuffers[currentFrame].Create( 
				device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( device, _debugLines.data(), size );
				VkCommandBuffer cmd = device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesvertexBuffers[currentFrame].buffer, size );
				device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( device );
			}
		}

		if( _debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth );
			m_debugLinesNoDepthTestVertexBuffers[currentFrame].Destroy( device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			m_debugLinesNoDepthTestVertexBuffers[currentFrame].Create(
				device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( device, _debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesNoDepthTestVertexBuffers[currentFrame].buffer, size );
				device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( device );
			}
		}

		if( _debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles );
			m_debugTrianglesvertexBuffers[currentFrame].Destroy( device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			m_debugTrianglesvertexBuffers[currentFrame].Create(
				device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( device, _debugTriangles.data(), size );
				VkCommandBuffer cmd = device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugTrianglesvertexBuffers[currentFrame].buffer, size );
				device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( device );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordAllCommandBuffers()
	{
		for( size_t i = 0; i < m_window.GetSwapChain().GetSwapchainImagesCount(); i++ )
		{
			RecordCommandBufferImgui( i );
			RecordCommandBufferGeometry( i );
			RecordCommandBufferDebug( i );
			RecordCommandBufferPostProcess( i );
			RecordCommandBufferUI( i );
			RecordPrimaryCommandBuffer( i );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordPrimaryCommandBuffer( const size_t _index )
	{
		SCOPED_PROFILE( primary );
		VkCommandBuffer commandBuffer = m_primaryCommandBuffers.buffers[ _index ];

		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( VK_NULL_HANDLE );

		std::vector<VkClearValue> clearValues( 2 );
		clearValues[ 0 ].color = { m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo =			  RenderPass::GetBeginInfo(	m_renderPassGame.renderPass,		m_gameFrameBuffers->Get( _index ),		  m_gameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoPostprocess = RenderPass::GetBeginInfo( m_renderPassPostprocess.renderPass, m_postProcessFramebuffers->Get( _index ), m_gameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoImGui =		  RenderPass::GetBeginInfo( m_renderPassImgui.renderPass,		m_swapchainFramebuffers->Get( _index ),	  m_window.GetSwapChain().GetExtent(), clearValues.data(), (uint32_t)clearValues.size() );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_geometryCommandBuffers.buffers[ _index ] );
				if ( ! m_hasNoDebugToDraw )
				{
					vkCmdExecuteCommands( commandBuffer, 1, &m_debugCommandBuffers.buffers[ _index ] );
				}
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_postprocessCommandBuffers.buffers[ _index ] );
				vkCmdExecuteCommands( commandBuffer, 1, &m_uiCommandBuffers.buffers[ _index ] );
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoImGui, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_imguiCommandBuffers.buffers[ _index ] );
			} vkCmdEndRenderPass( commandBuffer );


			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferPostProcess( const size_t _index )
	{

		VkCommandBuffer commandBuffer = m_postprocessCommandBuffers.buffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassPostprocess.renderPass, m_postProcessFramebuffers->Get( _index ) );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_postprocessPipeline->Bind( commandBuffer, _index );
			VkBuffer vertexBuffers[] = { m_quadVertexBuffer.buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
			vkCmdDraw( commandBuffer, static_cast< uint32_t >( 4 ), 1, 0, 0 );
			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferUI( const size_t _index )
	{
		VkCommandBuffer commandBuffer = m_uiCommandBuffers.buffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassPostprocess.renderPass, m_postProcessFramebuffers->Get( _index ) );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_uiPipeline->Bind( commandBuffer, _index );
			m_uiPipeline->BindDescriptors( commandBuffer, _index, 0 );

			VkDeviceSize offsets[] = { 0 };

			for ( uint32_t meshIndex = 0; meshIndex < m_uiMeshDrawArray.size(); meshIndex++ )
			{
				UIDrawData drawData = m_uiMeshDrawArray[ meshIndex ];
				UIMesh* mesh = drawData.mesh;
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer().buffer };
				m_uiPipeline->BindDescriptors( commandBuffer, _index, meshIndex );
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				BindTexture( commandBuffer, drawData.textureIndex, m_samplerDescriptorUI, m_uiPipeline->GetLayout() );
				vkCmdDraw( commandBuffer, static_cast< uint32_t >( mesh->GetVertices().size() ), 1, 0, 0 );
			}

			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferImgui( const size_t _index )
	{
		SCOPED_PROFILE( imgui );
		m_imguiPipeline->UpdateBuffer( _index );

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers.buffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassImgui.renderPass, m_swapchainFramebuffers->Get( _index ) );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_imguiPipeline->DrawFrame( commandBuffer, _index );

			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferGeometry( const size_t _index )
	{
		SCOPED_PROFILE( geometry );
		VkCommandBuffer commandBuffer = m_geometryCommandBuffers.buffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassGame.renderPass, m_gameFrameBuffers->Get( _index ) );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_forwardPipeline->Bind( commandBuffer, _index );

			for ( uint32_t meshIndex = 0; meshIndex < m_meshDrawArray.size(); meshIndex++ )
			{
				DrawData& drawData = m_meshDrawArray[ meshIndex ];
				BindTexture( commandBuffer, drawData.textureIndex, m_samplerDescriptorTextures, m_forwardPipeline->GetLayout() );
				m_forwardPipeline->BindDescriptors( commandBuffer, _index, meshIndex );
				VkDeviceSize offsets[] = { 0 };
				VkBuffer vertexBuffers[] = { drawData.mesh->GetVertexBuffer().buffer };
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				vkCmdBindIndexBuffer( commandBuffer, drawData.mesh->GetIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32 );
				vkCmdDrawIndexed( commandBuffer, static_cast< uint32_t >( drawData.mesh->GetIndices().size() ), 1, 0, 0, 0 );
			}

			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferDebug( const size_t _index )
	{
		SCOPED_PROFILE( debug );
		if( ! m_hasNoDebugToDraw )
		{
			VkCommandBuffer commandBuffer = m_debugCommandBuffers.buffers[_index];
			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassGame.renderPass, m_gameFrameBuffers->Get( _index ) );
			VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

			if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
			{
				VkDeviceSize offsets[] = { 0 };
				if( m_numDebugLines > 0 )
				{
					m_debugLinesPipeline->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesvertexBuffers[_index].buffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLines ), 1, 0, 0 );
				}
				if( m_numDebugLinesNoDepthTest > 0 )
				{
					m_debugLinesPipelineNoDepthTest->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesNoDepthTestVertexBuffers[_index].buffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLinesNoDepthTest ), 1, 0, 0 );
				}
				if( m_numDebugTriangle > 0 )
				{
					m_debugTrianglesPipeline->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugTrianglesvertexBuffers[_index].buffer };
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

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::SubmitCommandBuffers()
	{
		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		std::vector< VkCommandBuffer> commandBuffers = {
			m_primaryCommandBuffers.buffers[ m_window.GetSwapChain().GetCurrentImageIndex() ]
			//, m_imguiCommandBuffers[m_window.GetSwapChain().GetCurrentImageIndex()]
		};

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = m_window.GetSwapChain().GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount = static_cast< uint32_t >( commandBuffers.size() );
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = m_window.GetSwapChain().GetCurrentRenderFinishedSemaphore();

		VkResult result = vkQueueSubmit( m_window.GetDevice().GetGraphicsQueue(), 1, &submitInfo, *m_window.GetSwapChain().GetCurrentInFlightFence() );
		if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not submit draw command buffer " );
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders()
	{
		Debug::Highlight( "Reloading shaders" );

		vkDeviceWaitIdle( m_window.GetDevice().vkDevice );

		CreateTextureDescriptor();
		m_postprocessPipeline->ReloadShaders();
		m_forwardPipeline->ReloadShaders();
		m_debugLinesPipeline->ReloadShaders();
		m_debugLinesPipelineNoDepthTest->ReloadShaders();
		m_debugTrianglesPipeline->ReloadShaders();
		m_uiPipeline->ReloadShaders();

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadIcons()
	{
		WaitIdle();
		m_imguiPipeline->ReloadIcons();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateCommandBuffers()
	{
		const uint32_t count = m_window.GetSwapChain().GetSwapchainImagesCount();

		bool result = true;

		result &= m_primaryCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		result &= m_geometryCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_imguiCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_uiCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_postprocessCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_debugCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );

		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPasses()
	{
		VkDevice device = m_window.GetDevice().vkDevice;
		bool result = true;

		// game
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkAttachmentDescription depthAtt = RenderPass::GetDepthAttachment( m_window.GetDevice().FindDepthFormat() );
			VkAttachmentReference	depthAttRef = RenderPass::GetDepthAttachmentReference( 1 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, &depthAttRef );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			
			VkAttachmentDescription attachmentDescriptions[2] = { colorAtt, depthAtt };
			result &= m_renderPassGame.Create( device, attachmentDescriptions, 2, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// postprocess
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassPostprocess.Create( device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// imgui
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassImgui.Create( device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1);
		}

		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateFramebuffers()
	{
		const VkExtent2D extent = m_window.GetSwapChain().GetExtent();

		m_gameFrameBuffers = new FrameBuffer( m_window.GetDevice(), extent );
		m_gameFrameBuffers->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format, extent );
		m_gameFrameBuffers->AddDepthAttachment();
		m_gameFrameBuffers->Create( m_window.GetSwapChain().GetSwapchainImagesCount(), m_renderPassGame.renderPass );

		m_postProcessFramebuffers = new FrameBuffer( m_window.GetDevice(), extent );
		m_postProcessFramebuffers->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format, extent );
		m_postProcessFramebuffers->Create( m_window.GetSwapChain().GetSwapchainImagesCount(), m_renderPassPostprocess.renderPass );

		m_swapchainFramebuffers = new FrameBuffer( m_window.GetDevice(), extent );
		m_swapchainFramebuffers->SetExternalAttachment( m_window.GetSwapChain().GetImageViews() );
		m_swapchainFramebuffers->Create( m_window.GetSwapChain().GetSwapchainImagesCount(), m_renderPassImgui.renderPass );
	}

	//================================================================================================================================
	// Used for postprocess
	//================================================================================================================================
	void Renderer::CreateQuadVertexBuffer()
	{
		Device& device = m_window.GetDevice();

		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof( vertices[ 0 ] ) * vertices.size();
		m_quadVertexBuffer.Create(
			device,
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		Buffer stagingBuffer;
		stagingBuffer.Create(
			device,
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer.SetData( device, vertices.data(), size );
		VkCommandBuffer cmd = device.BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo( cmd, m_quadVertexBuffer.buffer, size );
		device.EndSingleTimeCommands( cmd );
		stagingBuffer.Destroy( device );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateTextureDescriptor()
	{		
		delete m_imagesDescriptor;

		const std::vector< Texture* >& texture = Texture::s_resourceManager.GetList();
		m_imagesDescriptor = new  DescriptorTextures( m_window.GetDevice(), static_cast< uint32_t >( texture.size() ) );

		std::vector< VkImageView > imageViews( texture.size() );
		for ( int textureIndex = 0; textureIndex < texture.size(); textureIndex++ )
		{
			m_imagesDescriptor->Append( texture[ textureIndex ]->imageView );
		}

		m_imagesDescriptor->UpdateRange( 0, m_imagesDescriptor->Count() - 1 );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler* _samplerDescriptor, VkPipelineLayout _pipelineLayout )
	{
		assert( _textureIndex < m_imagesDescriptor->Count() );

		std::vector<VkDescriptorSet> descriptors = {
			 m_imagesDescriptor->GetSet( _textureIndex )
			 , _samplerDescriptor->GetSet()
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			static_cast< uint32_t >( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}
}