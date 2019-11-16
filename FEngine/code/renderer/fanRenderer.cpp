#include "fanGlobalIncludes.h"
#include "renderer/fanRenderer.h"

#include "core/time/fanTime.h"
#include "core/input/fanInput.h"
#include "core/math/fanBasicModels.h"
#include "core/time/fanProfiler.h"
#include "renderer/fanMesh.h"
#include "renderer/fanUIMesh.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/core/fanInstance.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanSwapChain.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanImageView.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanFrameBuffer.h"
#include "renderer/pipelines/fanImguiPipeline.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/pipelines/fanDebugPipeline.h"
#include "renderer/pipelines/fanUIPipeline.h"
#include "renderer/util/fanVertex.h"
#include "renderer/util/fanWindow.h"
#include "renderer/descriptors/fanDescriptorTexture.h"
#include "renderer/core/fanSampler.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer( const VkExtent2D _size, const glm::ivec2 _position ) {
		m_instance =	new Instance();
		m_window =		new Window("FEngine", _size, _position, m_instance->vkInstance);
		m_device =		new Device(m_instance, m_window->GetSurface());

		m_swapchain =	new SwapChain(*m_device);		
		m_clearColor = glm::vec4(0.f, 0.f, 0.2f, 1.f);

		m_swapchain->Create(m_window->GetSurface(), _size);
		Input::Get().Setup(m_window->GetWindow());            
            
		CreateRenderPass();
		CreateRenderPassPostprocess();

		CreateQuadVertexBuffer();
		CreateSwapchainFramebuffers();
		CreateForwardFramebuffers();

		m_ressourceManager =  new RessourceManager( *m_device );
		m_sampler = new Sampler( *m_device );
		m_sampler->CreateSampler( 0, 8 );
		CreateTextureDescriptor();

		m_forwardPipeline = new ForwardPipeline(*m_device, m_texturesDescriptor);
		m_forwardPipeline->Init( m_renderPass, m_swapchain->GetExtent(), "code/shaders/forward.vert", "code/shaders/forward.frag" );
		m_forwardPipeline->CreateDescriptors( m_swapchain->GetSwapchainImagesCount(), m_ressourceManager );
		m_forwardPipeline->Create();

		m_debugLinesPipeline = new DebugPipeline(*m_device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true);
		m_debugLinesPipeline->Init( m_renderPass, m_swapchain->GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipeline->CreateDescriptors( m_swapchain->GetSwapchainImagesCount() );
		m_debugLinesPipeline->Create();

		m_debugLinesPipelineNoDepthTest = new DebugPipeline(*m_device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false);
		m_debugLinesPipelineNoDepthTest->Init( m_renderPass, m_swapchain->GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag" );
		m_debugLinesPipelineNoDepthTest->CreateDescriptors( m_swapchain->GetSwapchainImagesCount() );
		m_debugLinesPipelineNoDepthTest->Create();
            
		m_debugTrianglesPipeline = new DebugPipeline(*m_device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
		m_debugTrianglesPipeline->Init( m_renderPass, m_swapchain->GetExtent(), "code/shaders/debugTriangles.vert", "code/shaders/debugTriangles.frag" );
		m_debugTrianglesPipeline->CreateDescriptors( m_swapchain->GetSwapchainImagesCount() );
		m_debugTrianglesPipeline->Create();

		m_uiPipeline = new UIPipeline( *m_device, m_texturesDescriptor);
		m_uiPipeline->Init( m_renderPassPostprocess, m_swapchain->GetExtent(), "code/shaders/ui.vert", "code/shaders/ui.frag" );
		m_uiPipeline->CreateDescriptors( m_swapchain->GetSwapchainImagesCount() );
		m_uiPipeline->Create();

		m_postprocessPipeline = new PostprocessPipeline(*m_device);
		m_postprocessPipeline->SetImageAndView( m_forwardFrameBuffers->GetColorAttachmentImageView(), m_forwardFrameBuffers->GetColorAttachmentSampler() );
		m_postprocessPipeline->CreateDescriptors( m_swapchain->GetSwapchainImagesCount() );
		m_postprocessPipeline->Init( m_renderPassPostprocess, m_swapchain->GetExtent(), "code/shaders/postprocess.vert", "code/shaders/postprocess.frag" );
		m_postprocessPipeline->Create();
        
		m_imguiPipeline = new ImguiPipeline(*m_device, m_swapchain->GetSwapchainImagesCount());
		m_imguiPipeline->Create(m_renderPassPostprocess, m_window->GetWindow(), m_swapchain->GetExtent());

		


		CreateCommandBuffers();
		RecordAllCommandBuffers();

		m_debugLinesvertexBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		m_debugLinesNoDepthTestVertexBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		m_debugTrianglesvertexBuffers.resize(m_swapchain->GetSwapchainImagesCount());

		const size_t initialSize = 16;
		m_meshDrawArray.reserve( initialSize );
		m_forwardPipeline->m_dynamicUniformsVert.Resize( initialSize );
		m_forwardPipeline->m_dynamicUniformsMaterial.Resize( initialSize );
	}
	
	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer() {
		vkDeviceWaitIdle( m_device->vkDevice );

		delete m_imguiPipeline;
		delete m_forwardPipeline;
		delete m_debugLinesPipeline;
		delete m_debugLinesPipelineNoDepthTest;
		delete m_debugTrianglesPipeline;
		delete m_uiPipeline;
		delete m_ressourceManager;

		delete m_texturesDescriptor;
		delete m_sampler;

		delete m_quadVertexBuffer;

		for ( int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++ ) {
			delete m_debugLinesvertexBuffers[bufferIndex];
		} m_debugLinesvertexBuffers.clear();

		for ( int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++ ) {
			delete m_debugLinesNoDepthTestVertexBuffers[bufferIndex];
		} m_debugLinesNoDepthTestVertexBuffers.clear();

		for ( int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++ ) {
			delete m_debugTrianglesvertexBuffers[bufferIndex];
		} m_debugTrianglesvertexBuffers.clear();

		delete m_forwardFrameBuffers;
		DeleteRenderPass();
		DeleteRenderPassPostprocess();

		delete m_swapchainFramebuffers;

		delete m_postprocessPipeline;
		delete m_swapchain;
		delete m_device;
		delete m_window;
		delete m_instance;
	}
	
	//================================================================================================================================
	//================================================================================================================================	
	bool Renderer::WindowIsOpen() { 
		return ! glfwWindowShouldClose(m_window->GetWindow()); 
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame( ) {
		SCOPED_PROFILE( draw_frame )

		const VkResult result = m_swapchain->AcquireNextImage();
		if ( result == VK_ERROR_OUT_OF_DATE_KHR ) {

			// window minimized
			if ( m_window->GetExtent().width == 0 && m_window->GetExtent().height == 0 ) {
				glfwPollEvents();
				return;
			}
			vkDeviceWaitIdle( m_device->vkDevice );
			const VkExtent2D extent = m_window->GetExtent();
			Debug::Get() << Debug::Severity::highlight << "Resize renderer: " << extent.width << "x" << extent.height << Debug::Endl();
			m_swapchain->Resize( extent );
			m_swapchainFramebuffers->Resize( extent );
			m_forwardFrameBuffers->Resize( extent );
			m_postprocessPipeline->Resize( extent );
			m_forwardPipeline->Resize( extent );
			m_debugLinesPipeline->Resize( extent );
			m_debugLinesPipelineNoDepthTest->Resize( extent );
			m_debugTrianglesPipeline->Resize( extent );
			m_uiPipeline->Resize( extent );

			RecordAllCommandBuffers();
			vkResetFences( m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence() );
			m_swapchain->AcquireNextImage();
		} else if ( result != VK_SUCCESS ) {
			Debug::Error( "Could not acquire next image" );
		} else {
			vkWaitForFences( m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max() );
			vkResetFences( m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence() );
		}

		ImGui::GetIO().DisplaySize = ImVec2( static_cast<float>( m_swapchain->GetExtent().width ), static_cast<float>( m_swapchain->GetExtent().height ) );

		if ( m_ressourceManager->IsModified() ) {
			WaitIdle();
			CreateTextureDescriptor();
			m_ressourceManager->SetUnmodified();
		}

		const uint32_t currentFrame = m_swapchain->GetCurrentFrame();
		UpdateUniformBuffers( currentFrame );
		{
			SCOPED_PROFILE( record_cmd )
			RecordCommandBufferGeometry( currentFrame );
			RecordCommandBufferDebug( currentFrame );
			RecordCommandBufferUI( currentFrame );
			RecordCommandBufferImgui( currentFrame );
			RecordPrimaryCommandBuffer( currentFrame );
		}

		{
			SCOPED_PROFILE( submit )
			SubmitCommandBuffers();
			m_swapchain->PresentImage();
			m_swapchain->StartNextFrame();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffers( const size_t _index ) {
		m_postprocessPipeline->UpdateUniformBuffers( _index );
		m_forwardPipeline->UpdateUniformBuffers( _index );
		m_debugLinesPipeline->UpdateUniformBuffers( _index );
		m_debugLinesPipelineNoDepthTest->UpdateUniformBuffers( _index );
		m_debugTrianglesPipeline->UpdateUniformBuffers( _index );
		m_uiPipeline->UpdateUniformBuffers( _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::WaitIdle() { 
		vkDeviceWaitIdle(m_device->vkDevice); 
		Debug::Highlight("Renderer idle");
	}

	//================================================================================================================================
	//================================================================================================================================
	float  Renderer::GetWindowAspectRatio() const { 
		return static_cast<float>( m_swapchain->GetExtent().width ) / m_swapchain->GetExtent().height ;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position ) {
		m_forwardPipeline->m_vertUniforms.view = _view;
		m_forwardPipeline->m_vertUniforms.proj = _projection;
		m_forwardPipeline->m_vertUniforms.proj[1][1] *= -1;

		m_forwardPipeline->m_fragUniforms.cameraPosition = _position;

		std::array< DebugPipeline *, 3 > debugLinesPipelines  = { m_debugLinesPipeline, m_debugLinesPipelineNoDepthTest, m_debugTrianglesPipeline };
		for (int pipelingIndex = 0; pipelingIndex < debugLinesPipelines.size(); pipelingIndex++){
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.model = glm::mat4( 1.0 );
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.view = m_forwardPipeline->m_vertUniforms.view;
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.proj = m_forwardPipeline->m_vertUniforms.proj;
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDirectionalLight( const int _index, const glm::vec4 _direction, const glm::vec4 _ambiant, const glm::vec4 _diffuse, const glm::vec4 _specular ) {
		assert( _index  < GlobalValues::s_maximumNumDirectionalLight );
		m_forwardPipeline->m_lightUniforms.dirLights[_index].direction = _direction;
		m_forwardPipeline->m_lightUniforms.dirLights[_index].ambiant = _ambiant;
		m_forwardPipeline->m_lightUniforms.dirLights[_index].diffuse = _diffuse;
		m_forwardPipeline->m_lightUniforms.dirLights[_index].specular = _specular;			
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetNumDirectionalLights( const uint32_t _num ) {
		assert( _num < GlobalValues::s_maximumNumDirectionalLight );
		m_forwardPipeline->m_lightUniforms.dirLightsNum = _num;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetPointLight( const int _index, const glm::vec3 _position, const glm::vec3 _diffuse, const glm::vec3 _specular, const glm::vec3 _ambiant, const glm::vec3 _constantLinearQuadratic ) {
		assert( _index < GlobalValues::s_maximumNumPointLights );
		m_forwardPipeline->m_lightUniforms.pointlights[_index].position	= glm::vec4(_position,1);
		m_forwardPipeline->m_lightUniforms.pointlights[_index].diffuse		= glm::vec4( _diffuse, 1 );
		m_forwardPipeline->m_lightUniforms.pointlights[_index].specular	= glm::vec4( _specular, 1 );
		m_forwardPipeline->m_lightUniforms.pointlights[_index].ambiant		= glm::vec4( _ambiant, 1 );
		m_forwardPipeline->m_lightUniforms.pointlights[_index].constant	= _constantLinearQuadratic[0];
		m_forwardPipeline->m_lightUniforms.pointlights[_index].linear		= _constantLinearQuadratic[1];
		m_forwardPipeline->m_lightUniforms.pointlights[_index].quadratic	= _constantLinearQuadratic[2];
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetNumPointLights( const uint32_t _num ) {
		assert( _num < GlobalValues::s_maximumNumPointLights );
		m_forwardPipeline->m_lightUniforms.pointLightNum = _num;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDrawData( const std::vector<DrawMesh> & _drawData ) {
		if ( _drawData.size() > m_meshDrawArray.capacity() ) {
			Debug::Warning("Resizing draw data arrays");
			WaitIdle();
			const size_t newSize = 2 * _drawData.size();
			m_meshDrawArray.reserve( newSize );
			m_forwardPipeline->ResizeDynamicDescriptors( newSize );
		}

		m_meshDrawArray.clear();
		for (int dataIndex = 0; dataIndex < _drawData.size(); dataIndex++)	{
			const DrawMesh& data = _drawData[dataIndex];

			if ( data.mesh != nullptr && ! data.mesh->GetIndices().empty() ) {
				// Transform
				m_forwardPipeline->m_dynamicUniformsVert[dataIndex].modelMat = data.modelMatrix;
				m_forwardPipeline->m_dynamicUniformsVert[dataIndex].normalMat = data.normalMatrix;

				// material
				m_forwardPipeline->m_dynamicUniformsMaterial[dataIndex].color = data.color;
				m_forwardPipeline->m_dynamicUniformsMaterial[dataIndex].shininess = data.shininess;

				// Mesh
				m_meshDrawArray.push_back( {data.mesh, data.textureIndex } );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetUIDrawData( const std::vector<DrawUIMesh> & _drawData )
	{
		m_uiMeshDrawArray.resize( _drawData.size() );
		for (int meshIndex = 0; meshIndex < _drawData.size() ; meshIndex++)
		{
			const DrawUIMesh& uiData = _drawData[meshIndex];

			m_uiMeshDrawArray[meshIndex].mesh = uiData.mesh;
			m_uiMeshDrawArray[meshIndex].textureIndex = uiData.textureIndex;
			m_uiPipeline->m_dynamicUniformsVert[meshIndex].position = uiData.position;
			m_uiPipeline->m_dynamicUniformsVert[meshIndex].scale = uiData.scale;
			m_uiPipeline->m_dynamicUniformsVert[meshIndex].color = uiData.color;

		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordAllCommandBuffers() {
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_imguiCommandBuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferImgui(cmdBufferIndex);
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_geometryCommandBuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferGeometry(cmdBufferIndex);
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_debugCommandBuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferDebug(cmdBufferIndex);
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_swapchain->GetSwapchainImagesCount(); cmdBufferIndex++) {
			RecordCommandBufferPostProcess( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_uiCommandBuffers.size(); cmdBufferIndex++ )
		{
			RecordCommandBufferUI( cmdBufferIndex );
		}
		for ( size_t cmdBufferIndex = 0; cmdBufferIndex < m_primaryCommandBuffers.size(); cmdBufferIndex++) {
			RecordPrimaryCommandBuffer(cmdBufferIndex);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordPrimaryCommandBuffer(const size_t _index) {
		SCOPED_PROFILE( primary )
		VkCommandBuffer commandBuffer = m_primaryCommandBuffers[_index];

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.pNext = nullptr;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_forwardFrameBuffers->GetFrameBuffer( _index );
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent.width = m_swapchain->GetExtent().width;
		renderPassInfo.renderArea.extent.height = m_swapchain->GetExtent().height;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkRenderPassBeginInfo renderPassInfoPostprocess = {};
		renderPassInfoPostprocess.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfoPostprocess.pNext = nullptr;
		renderPassInfoPostprocess.renderPass = m_renderPassPostprocess;
		renderPassInfoPostprocess.framebuffer = m_swapchainFramebuffers->GetFrameBuffer( _index );
		renderPassInfoPostprocess.renderArea.offset = { 0,0 };
		renderPassInfoPostprocess.renderArea.extent.width = m_swapchain->GetExtent().width;
		renderPassInfoPostprocess.renderArea.extent.height = m_swapchain->GetExtent().height;
		renderPassInfoPostprocess.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfoPostprocess.pClearValues = clearValues.data();

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS); {
				vkCmdExecuteCommands(commandBuffer, 1, &m_geometryCommandBuffers[_index]);
				if (HasNoDebugToDraw() == false) {
					vkCmdExecuteCommands(commandBuffer, 1, &m_debugCommandBuffers[_index]);
				}
			} vkCmdEndRenderPass(commandBuffer);
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS); {
				vkCmdExecuteCommands( commandBuffer, 1, &m_postprocessCommandBuffers[_index]	);
				vkCmdExecuteCommands( commandBuffer, 1, &m_uiCommandBuffers[_index]	);
				vkCmdExecuteCommands( commandBuffer, 1, &m_imguiCommandBuffers[_index]			);
			} vkCmdEndRenderPass(commandBuffer);
			
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else {
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}

	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferPostProcess( const size_t _index ) {

		VkCommandBuffer commandBuffer = m_postprocessCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
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

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			m_postprocessPipeline->Bind(commandBuffer, _index );
			VkBuffer vertexBuffers[] = { m_quadVertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
			vkCmdDraw( commandBuffer, static_cast<uint32_t>( 4 ), 1, 0, 0 );
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
			}
		}
		else {
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferUI( const size_t _index )
	{
		VkCommandBuffer commandBuffer = m_uiCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
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
			m_uiPipeline->Bind( commandBuffer, _index );
			m_uiPipeline->BindDescriptors( commandBuffer, _index, 0 );
			
			VkDeviceSize offsets[] = { 0 };


			for ( uint32_t meshIndex = 0; meshIndex < m_uiMeshDrawArray.size(); meshIndex++ ) {
				UIDrawData drawData = m_uiMeshDrawArray[meshIndex];
				UIMesh * mesh = drawData.mesh;
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer()->GetBuffer() };
				m_uiPipeline->BindDescriptors( commandBuffer, _index, meshIndex );
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				BindTexture( commandBuffer, drawData.textureIndex, m_uiPipeline->GetLayout() );
				vkCmdDraw( commandBuffer, static_cast<uint32_t>( mesh->GetVertices().size() ), 1, 0, 0 );
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
	void Renderer::RecordCommandBufferImgui(const size_t _index) {
		SCOPED_PROFILE( imgui )
		m_imguiPipeline->UpdateBuffer(_index);

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
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

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			m_imguiPipeline->DrawFrame(commandBuffer, _index);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else {
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferDebug(const size_t _index) {
		SCOPED_PROFILE( debug )
		if (HasNoDebugToDraw() == false) {
			UpdateDebugBuffer(_index);

			VkCommandBuffer commandBuffer = m_debugCommandBuffers[_index];

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = m_renderPass;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = m_forwardFrameBuffers->GetFrameBuffer( _index );
			commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
			//commandBufferInheritanceInfo.queryFlags				=;
			//commandBufferInheritanceInfo.pipelineStatistics		=;

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

			if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {	
				VkDeviceSize offsets[] = { 0 }; 
				if( m_debugLines.size() > 0 ) {
					m_debugLinesPipeline->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesvertexBuffers[_index]->GetBuffer() };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_debugLines.size() ), 1, 0, 0 );
				}
				if ( m_debugLinesNoDepthTest.size() > 0 ) {
					m_debugLinesPipelineNoDepthTest->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesNoDepthTestVertexBuffers[_index]->GetBuffer() };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_debugLinesNoDepthTest.size() ), 1, 0, 0 );
				}
				if ( m_debugTriangles.size() > 0 ) {
					m_debugTrianglesPipeline->Bind( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { m_debugTrianglesvertexBuffers[_index]->GetBuffer() };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_debugTriangles.size() ), 1, 0, 0 );
				}
				if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
					Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
				}
			}
			else {
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
	}	

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferGeometry(const size_t _index) {
		SCOPED_PROFILE( geometry )
		VkCommandBuffer commandBuffer = m_geometryCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPass;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_forwardFrameBuffers->GetFrameBuffer( _index );
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			m_forwardPipeline->Bind( commandBuffer, _index );			

			for ( uint32_t meshIndex = 0; meshIndex < m_meshDrawArray.size(); meshIndex++ ) {
				DrawData& drawData = m_meshDrawArray[meshIndex];
				BindTexture( commandBuffer, drawData.textureIndex, m_forwardPipeline->GetLayout() );
				m_forwardPipeline->BindDescriptors( commandBuffer, _index, meshIndex );
				VkDeviceSize offsets[] = { 0 };
				VkBuffer vertexBuffers[] = { drawData.mesh->GetVertexBuffer()->GetBuffer() };
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				vkCmdBindIndexBuffer( commandBuffer, drawData.mesh->GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT32 );
				vkCmdDrawIndexed( commandBuffer, static_cast<uint32_t>( drawData.mesh->GetIndices().size() ), 1, 0, 0, 0 );
			}

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
		else {
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::SubmitCommandBuffers() {

		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		std::vector< VkCommandBuffer> commandBuffers = {
			m_primaryCommandBuffers[m_swapchain->GetCurrentImageIndex()]
			//, m_imguiCommandBuffers[m_swapchain->GetCurrentImageIndex()]
		};

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = m_swapchain->GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = m_swapchain->GetCurrentRenderFinishedSemaphore();

		VkResult result = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, *m_swapchain->GetCurrentInFlightFence());
		if (result != VK_SUCCESS) {
			Debug::Error( "Could not submit draw command buffer " );
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders() {
		Debug::Highlight("Reloading shaders");

		vkDeviceWaitIdle(m_device->vkDevice);

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
	void Renderer::UpdateDebugBuffer(const size_t _index) {
		SCOPED_PROFILE( update_buffer )
		if( m_debugLines.size() > 0) {
			SCOPED_PROFILE( lines )
			delete m_debugLinesvertexBuffers[_index];	// TODO update instead of delete
			const VkDeviceSize size = sizeof(DebugVertex) * m_debugLines.size();
			m_debugLinesvertexBuffers[_index] = new Buffer(*m_device);
			m_debugLinesvertexBuffers[_index]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if (size > 0) {
				Buffer stagingBuffer(*m_device);
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData(m_debugLines.data(), size);
				VkCommandBuffer cmd = m_device->BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo(cmd, m_debugLinesvertexBuffers[_index]->GetBuffer(), size);
				m_device->EndSingleTimeCommands(cmd);
			}
		}

		if (m_debugLinesNoDepthTest.size() > 0) {
			SCOPED_PROFILE( lines_no_depth )
			delete m_debugLinesNoDepthTestVertexBuffers[_index];
			const VkDeviceSize size = sizeof(DebugVertex) * m_debugLinesNoDepthTest.size();
			m_debugLinesNoDepthTestVertexBuffers[_index] = new Buffer(*m_device);
			m_debugLinesNoDepthTestVertexBuffers[_index]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if (size > 0) {
				Buffer stagingBuffer(*m_device);
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData(m_debugLinesNoDepthTest.data(), size);
				VkCommandBuffer cmd = m_device->BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo(cmd, m_debugLinesNoDepthTestVertexBuffers[_index]->GetBuffer(), size);
				m_device->EndSingleTimeCommands(cmd);
			}
		}

		if(m_debugTriangles.size() > 0 ){
			SCOPED_PROFILE( triangles )
			delete m_debugTrianglesvertexBuffers[_index];
			const VkDeviceSize size = sizeof(DebugVertex) * m_debugTriangles.size();
			m_debugTrianglesvertexBuffers[_index] = new Buffer(*m_device);
			m_debugTrianglesvertexBuffers[_index]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if (size > 0) {
				Buffer stagingBuffer(*m_device);
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData(m_debugTriangles.data(), size);
				VkCommandBuffer cmd = m_device->BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo(cmd, m_debugTrianglesvertexBuffers[_index]->GetBuffer(), size);
				m_device->EndSingleTimeCommands(cmd);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DebugPoint(const btVector3 _pos, const Color _color) {
		const float size = 0.2f;
		DebugLine(_pos - size * btVector3::Up(), _pos + size * btVector3::Up(), _color);
		DebugLine(_pos - size * btVector3::Left(), _pos + size * btVector3::Left(), _color);
		DebugLine(_pos - size * btVector3::Forward(), _pos + size * btVector3::Forward(), _color);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DebugLine(const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable) {
		if ( _depthTestEnable ) {
			m_debugLines.push_back(DebugVertex(ToGLM(_start), glm::vec3(0, 0, 0), _color.ToGLM()));
			m_debugLines.push_back(DebugVertex(ToGLM(_end), glm::vec3(0, 0, 0), _color.ToGLM()));
		} else {
			m_debugLinesNoDepthTest.push_back(DebugVertex(ToGLM(_start), glm::vec3(0, 0, 0), _color.ToGLM()));
			m_debugLinesNoDepthTest.push_back(DebugVertex(ToGLM(_end), glm::vec3(0, 0, 0), _color.ToGLM()));
		}

	}

	//================================================================================================================================
	// takes a list of triangle and a list of colors
	// 3 vertices per triangle
	// 1 color per triangle
	//================================================================================================================================
	void Renderer::DebugTriangles( const std::vector<btVector3>& _triangles, const std::vector<Color>& _colors ) {
		assert(_triangles .size() % 3 == 0 );
		assert( _colors.size() == _triangles.size() / 3 );

		m_debugTriangles.reserve(m_debugTriangles.size() + _triangles.size() );		
		for (int triangleIndex = 0; triangleIndex < _triangles.size() / 3; triangleIndex++)	{
			btVector3 v0 = _triangles[ 3 * triangleIndex + 0 ];
			btVector3 v1 = _triangles[ 3 * triangleIndex + 1 ];
			btVector3 v2 = _triangles[ 3 * triangleIndex + 2 ];

			const glm::vec3 normal = glm::normalize( ToGLM( ( v1 - v2 ).cross( v0 - v2 ) ) );
			m_debugTriangles.push_back( DebugVertex( ToGLM( v0 ), normal, _colors[triangleIndex].ToGLM() ) );
			m_debugTriangles.push_back( DebugVertex( ToGLM( v1 ), normal, _colors[triangleIndex].ToGLM() ) );
			m_debugTriangles.push_back( DebugVertex( ToGLM( v2 ), normal, _colors[triangleIndex].ToGLM() ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DebugCircle	  ( const btVector3 _pos, const float _radius, btVector3 _axis, uint32_t _nbSegments, const Color _color ) {
		assert( _nbSegments  > 2 && _radius >= 0.f);

		const btVector3 other = btVector3( -_axis[1], -_axis[2], _axis[0] );
		btVector3 orthogonal = _radius * _axis.cross(other).normalized();	
		const float angle = 2.f * PI / (float)_nbSegments;

		for ( uint32_t segmentIndex = 0; segmentIndex < _nbSegments ; segmentIndex++) {

			btVector3 start = _pos + orthogonal;
			orthogonal = orthogonal.rotate( _axis, angle );
			btVector3 end = _pos + orthogonal;			

			m_debugLines.push_back( DebugVertex( ToGLM( start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
			m_debugLines.push_back( DebugVertex( ToGLM( end ),   glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DebugTriangle(const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color) {
		const glm::vec3 normal = glm::normalize(ToGLM((_v1 - _v2).cross(_v0 - _v2)));
		m_debugTriangles.push_back(DebugVertex(ToGLM(_v0), normal, _color.ToGLM()));
		m_debugTriangles.push_back(DebugVertex(ToGLM(_v1), normal, _color.ToGLM()));
		m_debugTriangles.push_back(DebugVertex(ToGLM(_v2), normal, _color.ToGLM()));
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< btVector3> Renderer::DebugCube(const btTransform _transform, const btVector3 _halfExtent, const Color _color) {
		std::vector< btVector3 > square = GetCube(_halfExtent);		

		for (int vertIndex = 0; vertIndex < square.size(); vertIndex++)	{
			square[vertIndex] = _transform * square[vertIndex];
		}

		glm::vec4 glmColor = _color.ToGLM();

		for (int triangleIndex = 0; triangleIndex < square.size() / 3; triangleIndex++) {
			const glm::vec3 & v0 = ToGLM(square[3 * triangleIndex + 0]);
			const glm::vec3 & v1 = ToGLM(square[3 * triangleIndex + 1]);
			const glm::vec3 & v2 = ToGLM(square[3 * triangleIndex + 2]);

			m_debugLines.push_back( DebugVertex( v0, glm::vec3( 0, 0, 0 ), glmColor ) );
			m_debugLines.push_back( DebugVertex( v1, glm::vec3( 0, 0, 0 ), glmColor ) );
			m_debugLines.push_back( DebugVertex( v1, glm::vec3( 0, 0, 0 ), glmColor ) );
			m_debugLines.push_back( DebugVertex( v2, glm::vec3( 0, 0, 0 ), glmColor ) );
			m_debugLines.push_back( DebugVertex( v2, glm::vec3( 0, 0, 0 ), glmColor ) );
			m_debugLines.push_back( DebugVertex( v0, glm::vec3( 0, 0, 0 ), glmColor ) );

		}

		return square;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< btVector3> Renderer::DebugSphere(const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color) {
		if (_radius <= 0) {
			Debug::Warning("Debug sphere radius cannot be zero or negative");
			return {};
		}
			
		std::vector<btVector3> sphere = GetSphere(_radius, _numSubdivisions);

		for (int vertIndex = 0; vertIndex < sphere.size(); vertIndex++) {
			sphere[vertIndex] = _transform * sphere[vertIndex];
		}

		for (int triangleIndex = 0; triangleIndex < sphere.size() / 3; triangleIndex++) {
			const btVector3 v0 = sphere[3 * triangleIndex + 0];
			const btVector3 v1 = sphere[3 * triangleIndex + 1];
			const btVector3 v2 = sphere[3 * triangleIndex + 2];
			DebugLine(v0, v1, _color, false);
			DebugLine(v1, v2, _color, false );
			DebugLine(v2, v0, _color, false );
		}

		return sphere;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< btVector3> Renderer::DebugCone(const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color) {
		std::vector<btVector3> cone = GetCone(_radius, _height, _numSubdivisions);

		for (int vertIndex = 0; vertIndex < cone.size(); vertIndex++) {
			cone[vertIndex] = _transform * cone[vertIndex];
		}

		for (int triangleIndex = 0; triangleIndex < cone.size() / 3; triangleIndex++) {
			DebugTriangle(cone[3 * triangleIndex + 0], cone[3 * triangleIndex + 1], cone[3 * triangleIndex + 2], _color);
		}

		return cone;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DebugAABB(const AABB & _aabb, const Color _color) {
		std::vector< btVector3 > corners = _aabb.GetCorners();
		// Top
		DebugLine(corners[0], corners[1], _color);
		DebugLine(corners[1], corners[2], _color);
		DebugLine(corners[2], corners[3], _color);
		DebugLine(corners[3], corners[0], _color);
		// Bot
		DebugLine(corners[4], corners[5], _color);
		DebugLine(corners[5], corners[6], _color);
		DebugLine(corners[6], corners[7], _color);
		DebugLine(corners[7], corners[4], _color);
		//Vertical sides
		DebugLine(corners[0], corners[4], _color);
		DebugLine(corners[1], corners[5], _color);
		DebugLine(corners[2], corners[6], _color);
		DebugLine(corners[3], corners[7], _color);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateCommandBuffers() {
		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = m_device->GetCommandPool();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = m_swapchain->GetSwapchainImagesCount();

		m_primaryCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());

		if (vkAllocateCommandBuffers(m_device->vkDevice, &commandBufferAllocateInfo, m_primaryCommandBuffers.data()) != VK_SUCCESS) {
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		VkCommandBufferAllocateInfo secondaryCommandBufferAllocateInfo;
		secondaryCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		secondaryCommandBufferAllocateInfo.pNext = nullptr;
		secondaryCommandBufferAllocateInfo.commandPool = m_device->GetCommandPool();
		secondaryCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		secondaryCommandBufferAllocateInfo.commandBufferCount = m_swapchain->GetSwapchainImagesCount();

		m_geometryCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_geometryCommandBuffers.data()) != VK_SUCCESS) {
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		m_debugCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_debugCommandBuffers.data()) != VK_SUCCESS) {
			Debug::Error( "Could not allocate debug command buffers." );
			return false;
		}

		m_imguiCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_imguiCommandBuffers.data()) != VK_SUCCESS) {
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		m_uiCommandBuffers.resize( m_swapchain->GetSwapchainImagesCount() );
		if ( vkAllocateCommandBuffers( m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_uiCommandBuffers.data() ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		m_postprocessCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_postprocessCommandBuffers.data()) != VK_SUCCESS) {
			Debug::Error( "Could not allocate command buffers." );
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPass() {
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = m_swapchain->GetSurfaceFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription depthAttachment;
		depthAttachment.flags = 0;
		depthAttachment.format = m_device->FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference>   inputAttachments = {};
		std::vector<VkAttachmentReference>   colorAttachments = { colorAttachmentRef };

		VkSubpassDescription subpassDescription;
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
		subpassDescription.pInputAttachments = inputAttachments.data();
		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpassDescription.pColorAttachments = colorAttachments.data();
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dependencyFlags = 0;


		std::vector<VkAttachmentDescription> attachmentsDescriptions = { colorAttachment, depthAttachment };
		std::vector<VkSubpassDescription> subpassDescriptions = { subpassDescription };
		std::vector<VkSubpassDependency> subpassDependencies = { dependency };

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentsDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());;
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());;
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (vkCreateRenderPass(m_device->vkDevice, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			Debug::Error( "Could not create render pass" );
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkRenderPass          " << m_renderPass << std::dec << Debug::Endl();

		return true;
	}	

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPassPostprocess() {
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = m_swapchain->GetSurfaceFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference>   inputAttachments = {};
		std::vector<VkAttachmentReference>   colorAttachments = { colorAttachmentRef };

		VkSubpassDescription subpassDescription;
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
		subpassDescription.pInputAttachments = inputAttachments.data();
		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpassDescription.pColorAttachments = colorAttachments.data();
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dependencyFlags = 0;

		std::vector<VkAttachmentDescription> attachmentsDescriptions = { colorAttachment };
		std::vector<VkSubpassDescription> subpassDescriptions = { subpassDescription };
		std::vector<VkSubpassDependency> subpassDependencies = { dependency };

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentsDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());;
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());;
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (vkCreateRenderPass(m_device->vkDevice, &renderPassCreateInfo, nullptr, &m_renderPassPostprocess) != VK_SUCCESS) {
			Debug::Error( "Could not create render pass pp" );
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkRenderPass pp       " << m_renderPassPostprocess << std::dec << Debug::Endl();

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateForwardFramebuffers() {
		m_forwardFrameBuffers = new FrameBuffer( *m_device );
		m_forwardFrameBuffers->AddColorAttachment( m_swapchain->GetExtent(), m_swapchain->GetSurfaceFormat().format );
		m_forwardFrameBuffers->AddDepthAttachment( m_swapchain->GetExtent() );
		m_forwardFrameBuffers->Create( m_swapchain->GetSwapchainImagesCount() , m_renderPass, m_swapchain->GetExtent() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateSwapchainFramebuffers() {	
		m_swapchainFramebuffers = new FrameBuffer( *m_device );
		m_swapchainFramebuffers->SetExternalAttachment( m_swapchain->GetImageViews() );
		m_swapchainFramebuffers->Create( m_swapchain->GetSwapchainImagesCount(), m_renderPassPostprocess, m_swapchain->GetExtent() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteRenderPass() {
		if (m_renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_device->vkDevice, m_renderPass, nullptr);
			m_renderPass = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteRenderPassPostprocess() {
		if (m_renderPassPostprocess != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_device->vkDevice, m_renderPassPostprocess, nullptr);
			m_renderPassPostprocess = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ClearDebug()
	{
		m_debugLines.clear();
		m_debugLinesNoDepthTest.clear();
		m_debugTriangles.clear();
	}

	//================================================================================================================================
	// Used for postprocess
	//================================================================================================================================
	void Renderer::CreateQuadVertexBuffer() {
		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof( vertices[0] ) * vertices.size();
		m_quadVertexBuffer = new Buffer( *m_device );
		m_quadVertexBuffer->Create(
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		Buffer stagingBuffer(*m_device );
		stagingBuffer.Create(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer.SetData( vertices.data(), size );
		VkCommandBuffer cmd = m_device->BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo( cmd, m_quadVertexBuffer->GetBuffer(), size );
		m_device->EndSingleTimeCommands( cmd );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateTextureDescriptor()
	{
		delete m_texturesDescriptor;

		const std::vector< Texture * > & texture = m_ressourceManager->GetTextures();
		m_texturesDescriptor = new  DescriptorTextures( *m_device, m_sampler->GetSampler(), static_cast<uint32_t>( texture.size() ) );

		std::vector< VkImageView > imageViews( texture.size() );
		for ( int textureIndex = 0; textureIndex < texture.size(); textureIndex++ )
		{
			m_texturesDescriptor->Append( texture[textureIndex]->GetImageView() );
		}

		m_texturesDescriptor->UpdateRange( 0, m_texturesDescriptor->Count() - 1 );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, VkPipelineLayout _pipelineLayout )
	{
		assert( _textureIndex < m_texturesDescriptor->Count() );

		std::vector<VkDescriptorSet> descriptors = {
			 m_texturesDescriptor->GetSet( _textureIndex )
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}
}