#include "render/fanRenderer.hpp"
#include "core/time/fanTime.hpp"
#include "core/input/fanInput.hpp"
#include "core/time/fanProfiler.hpp"
#include "render/fanRendererDebug.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/fanMeshManager.hpp"
#include "render/fanUIMesh.hpp"
#include "render/fanMesh.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanSwapChain.hpp"
#include "render/core/fanInstance.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/pipelines/fanPostprocessPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/pipelines/fanImguiPipeline.hpp"
#include "render/pipelines/fanUIPipeline.hpp"
#include "render/util/fanVertex.hpp"
#include "render/util/fanWindow.hpp"
#include "render/descriptors/fanDescriptorTexture.hpp"
#include "render/descriptors/fanDescriptorSampler.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer( Window& _window ) : m_window( _window )
	{
		m_rendererDebug = new RendererDebug( m_window.GetDevice(), m_window.GetSwapChain() );

		m_clearColor = glm::vec4( 0.f, 0.f, 0.2f, 1.f );

		Mesh::s_resourceManager.Init( m_window.GetDevice() );
		Texture::s_resourceManager.Init( m_window.GetDevice() );
		UIMesh::s_resourceManager.Init( m_window.GetDevice() );

		CreateRenderPass();
		CreateRenderPassPostprocess();
		CreateRenderPassImGui();

		CreateQuadVertexBuffer();
		CreateFramebuffers();

		m_samplerTextures = new Sampler( m_window.GetDevice() );
		m_samplerTextures->CreateSampler( 0, 8, VK_FILTER_LINEAR );
		m_samplerDescriptorTextures = new DescriptorSampler( m_window.GetDevice(), m_samplerTextures->GetSampler() );
		CreateTextureDescriptor();

		m_rendererDebug->Create( m_renderPassGame->GetRenderPass(), m_gameFrameBuffers );

		m_forwardPipeline = new ForwardPipeline( m_window.GetDevice(), m_imagesDescriptor, m_samplerDescriptorTextures );
		m_forwardPipeline->Init( m_renderPassGame->GetRenderPass(), m_window.GetSwapChain().GetExtent(), "code/shaders/forward.vert", "code/shaders/forward.frag" );
		m_forwardPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_forwardPipeline->Create();

		m_samplerUI = new Sampler( m_window.GetDevice() );
		m_samplerUI->CreateSampler( 0, 1, VK_FILTER_NEAREST );
		m_samplerDescriptorUI = new DescriptorSampler( m_window.GetDevice(), m_samplerUI->GetSampler() );
		m_uiPipeline = new UIPipeline( m_window.GetDevice(), m_imagesDescriptor, m_samplerDescriptorUI );
		m_uiPipeline->Init( m_renderPassPostprocess->GetRenderPass(), m_window.GetSwapChain().GetExtent(), "code/shaders/ui.vert", "code/shaders/ui.frag" );
		m_uiPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_uiPipeline->Create();

		m_postprocessPipeline = new PostprocessPipeline( m_window.GetDevice() );
		m_postprocessPipeline->SetGameImageView( m_gameFrameBuffers->GetColorAttachmentImageView() );
		m_postprocessPipeline->CreateDescriptors( m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_postprocessPipeline->Init( m_renderPassPostprocess->GetRenderPass(), m_window.GetSwapChain().GetExtent(), "code/shaders/postprocess.vert", "code/shaders/postprocess.frag" );
		m_postprocessPipeline->Create();

		m_imguiPipeline = new ImguiPipeline( m_window.GetDevice(), m_window.GetSwapChain().GetSwapchainImagesCount() );
		m_imguiPipeline->SetGameView( m_postProcessFramebuffers->GetColorAttachmentImageView() );
		m_imguiPipeline->Create( m_renderPassImgui->GetRenderPass(), m_window.GetWindow(), m_window.GetSwapChain().GetExtent() );

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
		vkDeviceWaitIdle( m_window.GetDevice().vkDevice );

		delete m_imguiPipeline;
		delete m_forwardPipeline;
		delete m_uiPipeline;

		Mesh::s_resourceManager.Clear();
		Texture::s_resourceManager.Clear();
		UIMesh::s_resourceManager.Clear();

		delete m_samplerDescriptorTextures;
		delete m_samplerDescriptorUI;
		delete m_imagesDescriptor;

		delete m_samplerTextures;
		delete m_samplerUI;

		delete m_quadVertexBuffer;

		delete m_rendererDebug;

		delete m_gameFrameBuffers;
		delete m_postProcessFramebuffers;
		delete m_renderPassGame;
		delete m_renderPassPostprocess;
		delete m_renderPassImgui;

		delete m_swapchainFramebuffers;

		delete m_postprocessPipeline;
	}



	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame()
	{
		SCOPED_PROFILE( draw_frame )

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
			SCOPED_PROFILE( record_cmd )
				RecordCommandBufferGeometry( currentFrame );
			m_rendererDebug->RecordCommandBufferDebug( currentFrame );
			RecordCommandBufferUI( currentFrame );
			RecordCommandBufferImgui( currentFrame );
			RecordPrimaryCommandBuffer( currentFrame );
		}

		{
			SCOPED_PROFILE( submit )
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
		m_rendererDebug->Resize( extent );
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
		m_rendererDebug->UpdateUniformBuffers( _index );
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

		m_rendererDebug->SetMainCamera( m_forwardPipeline->m_vertUniforms.proj, _view );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDirectionalLight( const int _index, const glm::vec4 _direction, const glm::vec4 _ambiant, const glm::vec4 _diffuse, const glm::vec4 _specular )
	{
		assert( _index < RenderGlobal::s_maximumNumDirectionalLight );
		m_forwardPipeline->m_lightUniforms.dirLights[ _index ].direction = _direction;
		m_forwardPipeline->m_lightUniforms.dirLights[ _index ].ambiant = _ambiant;
		m_forwardPipeline->m_lightUniforms.dirLights[ _index ].diffuse = _diffuse;
		m_forwardPipeline->m_lightUniforms.dirLights[ _index ].specular = _specular;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetNumDirectionalLights( const uint32_t _num )
	{
		assert( _num < RenderGlobal::s_maximumNumDirectionalLight );
		m_forwardPipeline->m_lightUniforms.dirLightsNum = _num;
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
	void Renderer::RecordAllCommandBuffers()
	{
		uint32_t imagesCount = m_window.GetSwapChain().GetSwapchainImagesCount();

		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_imguiCommandBuffers.size(); cmdBufferIndex++ )
		{
			RecordCommandBufferImgui( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_geometryCommandBuffers.size(); cmdBufferIndex++ )
		{
			RecordCommandBufferGeometry( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < imagesCount; cmdBufferIndex++ )
		{
			m_rendererDebug->RecordCommandBufferDebug( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < imagesCount; cmdBufferIndex++ )
		{
			RecordCommandBufferPostProcess( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_uiCommandBuffers.size(); cmdBufferIndex++ )
		{
			RecordCommandBufferUI( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_primaryCommandBuffers.size(); cmdBufferIndex++ )
		{
			RecordPrimaryCommandBuffer( cmdBufferIndex );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordPrimaryCommandBuffer( const size_t _index )
	{
		SCOPED_PROFILE( primary )
			VkCommandBuffer commandBuffer = m_primaryCommandBuffers[ _index ];

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		std::vector<VkClearValue> clearValues( 2 );
		clearValues[ 0 ].color = { m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.pNext = nullptr;
		renderPassInfo.renderPass = m_renderPassGame->GetRenderPass();
		renderPassInfo.framebuffer = m_gameFrameBuffers->GetFrameBuffer( _index );
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent.width = m_gameExtent.width;
		renderPassInfo.renderArea.extent.height = m_gameExtent.height;
		renderPassInfo.clearValueCount = static_cast< uint32_t >( clearValues.size() );
		renderPassInfo.pClearValues = clearValues.data();

		VkRenderPassBeginInfo renderPassInfoPostprocess = {};
		renderPassInfoPostprocess.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfoPostprocess.pNext = nullptr;
		renderPassInfoPostprocess.renderPass = m_renderPassPostprocess->GetRenderPass();
		renderPassInfoPostprocess.framebuffer = m_postProcessFramebuffers->GetFrameBuffer( _index );
		renderPassInfoPostprocess.renderArea.offset = { 0,0 };
		renderPassInfoPostprocess.renderArea.extent.width = m_gameExtent.width;
		renderPassInfoPostprocess.renderArea.extent.height = m_gameExtent.height;
		renderPassInfoPostprocess.clearValueCount = static_cast< uint32_t >( clearValues.size() );
		renderPassInfoPostprocess.pClearValues = clearValues.data();

		VkRenderPassBeginInfo renderPassInfoImGui = {};
		renderPassInfoImGui.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfoImGui.pNext = nullptr;
		renderPassInfoImGui.renderPass = m_renderPassImgui->GetRenderPass();
		renderPassInfoImGui.framebuffer = m_swapchainFramebuffers->GetFrameBuffer( _index );
		renderPassInfoImGui.renderArea.offset = { 0,0 };
		renderPassInfoImGui.renderArea.extent.width = m_window.GetSwapChain().GetExtent().width;
		renderPassInfoImGui.renderArea.extent.height = m_window.GetSwapChain().GetExtent().height;
		renderPassInfoImGui.clearValueCount = static_cast< uint32_t >( clearValues.size() );
		renderPassInfoImGui.pClearValues = clearValues.data();

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_geometryCommandBuffers[ _index ] );
				if ( !m_rendererDebug->HasNoDebugToDraw() )
				{
					vkCmdExecuteCommands( commandBuffer, 1, m_rendererDebug->GetCommandBuffer( _index ) );
				}
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_postprocessCommandBuffers[ _index ] );
				vkCmdExecuteCommands( commandBuffer, 1, &m_uiCommandBuffers[ _index ] );
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoImGui, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_imguiCommandBuffers[ _index ] );
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

		VkCommandBuffer commandBuffer = m_postprocessCommandBuffers[ _index ];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess->GetRenderPass();
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_postProcessFramebuffers->GetFrameBuffer( _index );
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_postprocessPipeline->Bind( commandBuffer, _index );
			VkBuffer vertexBuffers[] = { m_quadVertexBuffer->GetBuffer() };
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
		VkCommandBuffer commandBuffer = m_uiCommandBuffers[ _index ];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess->GetRenderPass();
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_postProcessFramebuffers->GetFrameBuffer( _index );
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_uiPipeline->Bind( commandBuffer, _index );
			m_uiPipeline->BindDescriptors( commandBuffer, _index, 0 );

			VkDeviceSize offsets[] = { 0 };


			for ( uint32_t meshIndex = 0; meshIndex < m_uiMeshDrawArray.size(); meshIndex++ )
			{
				UIDrawData drawData = m_uiMeshDrawArray[ meshIndex ];
				UIMesh* mesh = drawData.mesh;
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer()->GetBuffer() };
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
		SCOPED_PROFILE( imgui )
			m_imguiPipeline->UpdateBuffer( _index );

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers[ _index ];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassImgui->GetRenderPass();
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_swapchainFramebuffers->GetFrameBuffer( _index );
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

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
		SCOPED_PROFILE( geometry )
			VkCommandBuffer commandBuffer = m_geometryCommandBuffers[ _index ];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassGame->GetRenderPass();
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_gameFrameBuffers->GetFrameBuffer( _index );
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_forwardPipeline->Bind( commandBuffer, _index );

			for ( uint32_t meshIndex = 0; meshIndex < m_meshDrawArray.size(); meshIndex++ )
			{
				DrawData& drawData = m_meshDrawArray[ meshIndex ];
				BindTexture( commandBuffer, drawData.textureIndex, m_samplerDescriptorTextures, m_forwardPipeline->GetLayout() );
				m_forwardPipeline->BindDescriptors( commandBuffer, _index, meshIndex );
				VkDeviceSize offsets[] = { 0 };
				VkBuffer vertexBuffers[] = { drawData.mesh->GetVertexBuffer()->GetBuffer() };
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				vkCmdBindIndexBuffer( commandBuffer, drawData.mesh->GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT32 );
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
	bool Renderer::SubmitCommandBuffers()
	{

		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		std::vector< VkCommandBuffer> commandBuffers = {
			m_primaryCommandBuffers[ m_window.GetSwapChain().GetCurrentImageIndex() ]
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
		m_rendererDebug->ReloadShaders();
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
		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = m_window.GetDevice().GetCommandPool();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = m_window.GetSwapChain().GetSwapchainImagesCount();

		m_primaryCommandBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );

		if ( vkAllocateCommandBuffers( m_window.GetDevice().vkDevice, &commandBufferAllocateInfo, m_primaryCommandBuffers.data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		VkCommandBufferAllocateInfo secondaryCommandBufferAllocateInfo;
		secondaryCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		secondaryCommandBufferAllocateInfo.pNext = nullptr;
		secondaryCommandBufferAllocateInfo.commandPool = m_window.GetDevice().GetCommandPool();
		secondaryCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		secondaryCommandBufferAllocateInfo.commandBufferCount = m_window.GetSwapChain().GetSwapchainImagesCount();

		m_geometryCommandBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		if ( vkAllocateCommandBuffers( m_window.GetDevice().vkDevice, &secondaryCommandBufferAllocateInfo, m_geometryCommandBuffers.data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		m_rendererDebug->GetCommandBuffers().resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		if ( vkAllocateCommandBuffers( m_window.GetDevice().vkDevice, &secondaryCommandBufferAllocateInfo, m_rendererDebug->GetCommandBuffers().data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate debug command buffers." );
			return false;
		}

		m_imguiCommandBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		if ( vkAllocateCommandBuffers( m_window.GetDevice().vkDevice, &secondaryCommandBufferAllocateInfo, m_imguiCommandBuffers.data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		m_uiCommandBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		if ( vkAllocateCommandBuffers( m_window.GetDevice().vkDevice, &secondaryCommandBufferAllocateInfo, m_uiCommandBuffers.data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		m_postprocessCommandBuffers.resize( m_window.GetSwapChain().GetSwapchainImagesCount() );
		if ( vkAllocateCommandBuffers( m_window.GetDevice().vkDevice, &secondaryCommandBufferAllocateInfo, m_postprocessCommandBuffers.data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPass()
	{
		m_renderPassGame = new RenderPass( m_window.GetDevice() );
		m_renderPassGame->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format );
		m_renderPassGame->AddDepthAttachment( m_window.GetDevice().FindDepthFormat() );
		m_renderPassGame->AddDependency();
		return m_renderPassGame->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPassPostprocess()
	{
		m_renderPassPostprocess = new RenderPass( m_window.GetDevice() );
		m_renderPassPostprocess->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format );
		m_renderPassPostprocess->AddDependency();
		return m_renderPassPostprocess->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPassImGui()
	{
		m_renderPassImgui = new RenderPass( m_window.GetDevice() );
		m_renderPassImgui->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
		m_renderPassImgui->AddDependency();
		return m_renderPassImgui->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateFramebuffers()
	{
		m_gameFrameBuffers = new FrameBuffer( m_window.GetDevice(), m_window.GetSwapChain().GetExtent() );
		m_gameFrameBuffers->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format );
		m_gameFrameBuffers->AddDepthAttachment();
		m_gameFrameBuffers->Create( m_window.GetSwapChain().GetSwapchainImagesCount(), m_renderPassGame->GetRenderPass() );

		m_postProcessFramebuffers = new FrameBuffer( m_window.GetDevice(), m_window.GetSwapChain().GetExtent() );
		m_postProcessFramebuffers->AddColorAttachment( m_window.GetSwapChain().GetSurfaceFormat().format );
		m_postProcessFramebuffers->Create( m_window.GetSwapChain().GetSwapchainImagesCount(), m_renderPassPostprocess->GetRenderPass() );

		m_swapchainFramebuffers = new FrameBuffer( m_window.GetDevice(), m_window.GetSwapChain().GetExtent() );
		m_swapchainFramebuffers->SetExternalAttachment( m_window.GetSwapChain().GetImageViews() );
		m_swapchainFramebuffers->Create( m_window.GetSwapChain().GetSwapchainImagesCount(), m_renderPassImgui->GetRenderPass() );
	}

	//================================================================================================================================
	// Used for postprocess
	//================================================================================================================================
	void Renderer::CreateQuadVertexBuffer()
	{
		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof( vertices[ 0 ] ) * vertices.size();
		m_quadVertexBuffer = new Buffer( m_window.GetDevice() );
		m_quadVertexBuffer->Create(
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		Buffer stagingBuffer( m_window.GetDevice() );
		stagingBuffer.Create(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer.SetData( vertices.data(), size );
		VkCommandBuffer cmd = m_window.GetDevice().BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo( cmd, m_quadVertexBuffer->GetBuffer(), size );
		m_window.GetDevice().EndSingleTimeCommands( cmd );
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
			m_imagesDescriptor->Append( texture[ textureIndex ]->GetImageView() );
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