#include "fanGlobalIncludes.h"

#include "renderer/fanRenderer.h"
#include "scene/fanEntity.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanPointLight.h"
#include "core/fanTime.h"
#include "core/input/fanInput.h"
#include "core/math/fanBasicModels.h"
#include "renderer/fanMesh.h"
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
#include "renderer/util/fanVertex.h"
#include "renderer/util/fanWindow.h"

namespace fan

{
		//================================================================================================================================
		//================================================================================================================================
		void Renderer::Initialize(const VkExtent2D _size, const glm::ivec2 _position) {
			m_instance =	new Instance();
			m_window =		new Window("Vulkan", _size, _position, m_instance->vkInstance);
			m_device =		new Device(m_instance, m_window->GetSurface());
			m_swapchain =	new SwapChain(*m_device);
		
			m_clearColor = glm::vec4(0.f, 0.f, 0.2f, 1.f);

			m_swapchain->Create(m_window->GetSurface(), _size);
			Input::Get().Setup(m_window->GetWindow());

			CreateCommandPool();
			CreateRenderPass();
			CreateRenderPassPostprocess();

			m_ressourceManager =  new RessourceManager( *m_device );

			m_forwardPipeline = new ForwardPipeline(*m_device, m_renderPass);
			m_forwardPipeline->Create( m_swapchain->GetExtent());

			m_debugLinesPipeline = new DebugPipeline(*m_device, m_renderPass, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true);
			m_debugLinesPipeline->Create(m_swapchain->GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag");

			m_debugLinesPipelineNoDepthTest = new DebugPipeline(*m_device, m_renderPass, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false);
			m_debugLinesPipelineNoDepthTest->Create(m_swapchain->GetExtent(), "code/shaders/debugLines.vert", "code/shaders/debugLines.frag");
			
			m_debugTrianglesPipeline = new DebugPipeline(*m_device, m_renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
			m_debugTrianglesPipeline->Create(m_swapchain->GetExtent(), "code/shaders/debugTriangles.vert", "code/shaders/debugTriangles.frag");

			m_postprocessPipeline = new PostprocessPipeline(*m_device, m_renderPassPostprocess);
			m_postprocessPipeline->Create(m_swapchain->GetSurfaceFormat().format, m_swapchain->GetExtent());		
		
			m_imguiPipeline = new ImguiPipeline(*m_device, m_swapchain->GetSwapchainImagesCount());
			m_imguiPipeline->Create(m_renderPassPostprocess, m_window->GetWindow(), m_swapchain->GetExtent());

			CreateSwapchainFramebuffers();
			CreateForwardFramebuffers();
			CreateCommandBuffers();
			RecordAllCommandBuffers();

			m_debugLinesvertexBuffers.resize(m_swapchain->GetSwapchainImagesCount());
			m_debugLinesNoDepthTestVertexBuffers.resize(m_swapchain->GetSwapchainImagesCount());
			m_debugTrianglesvertexBuffers.resize(m_swapchain->GetSwapchainImagesCount());

			ImGui::NewFrame();
		}
	
		//================================================================================================================================
		//================================================================================================================================	
		void Renderer::Destroy() {
			vkDeviceWaitIdle(m_device->vkDevice);

			ImGui::EndFrame();

			delete m_imguiPipeline;
			delete m_forwardPipeline;
			delete m_debugLinesPipeline;
			delete m_debugLinesPipelineNoDepthTest;
			delete m_debugTrianglesPipeline;
			delete m_ressourceManager;

			for (int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++) {
				delete m_debugLinesvertexBuffers[bufferIndex];
			} m_debugLinesvertexBuffers.clear();

			for (int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++) {
				delete m_debugLinesNoDepthTestVertexBuffers[bufferIndex];
			} m_debugLinesNoDepthTestVertexBuffers.clear();

			for (int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++) {
				delete m_debugTrianglesvertexBuffers[bufferIndex];
			} m_debugTrianglesvertexBuffers.clear();

			DeleteForwardFramebuffers();
			DeleteRenderPass();
			DeleteRenderPassPostprocess();
			DeleteCommandPool();
			DeleteSwapchainFramebuffers();

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
				const VkResult result = m_swapchain->AcquireNextImage();
				if (result == VK_ERROR_OUT_OF_DATE_KHR ) {

					// window minimized
					if (m_window->GetExtent().width == 0 && m_window->GetExtent().height == 0) {
						glfwPollEvents();
						return;
					}

					Debug::Log( "suboptimal swapchain" );
					vkDeviceWaitIdle(m_device->vkDevice);

					DeleteForwardFramebuffers();
					DeleteSwapchainFramebuffers();

					m_swapchain->Resize(m_window->GetExtent());
					m_postprocessPipeline->Resize(m_window->GetExtent());
					m_forwardPipeline->Resize(m_window->GetExtent());
					m_debugLinesPipeline->Resize(m_window->GetExtent());
					m_debugLinesPipelineNoDepthTest->Resize(m_window->GetExtent());
					m_debugTrianglesPipeline->Resize(m_window->GetExtent());

					CreateSwapchainFramebuffers();
					CreateForwardFramebuffers();
					RecordAllCommandBuffers();
					vkResetFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
					m_swapchain->AcquireNextImage();	
					UpdateSceneUniforms();
				}
				else if (result != VK_SUCCESS) {
					Debug::Error( "Could not acquire next image" );
				}
				else {
					vkWaitForFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
					vkResetFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
				}
			
				ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(m_swapchain->GetExtent().width), static_cast<float>(m_swapchain->GetExtent().height));
				{
					ImGui::Begin("RendererDebugTmp"); {
						// Display mesh list
						if (ImGui::CollapsingHeader("Loaded meshes : ")) {
						
							for (auto meshData : m_ressourceManager->GetMeshData()) {
								ImGui::Text(meshData.second.mesh->GetPath().c_str());
							}
						}
						// display textures list
						const std::vector< Texture * > & textures = m_ressourceManager->GetTextures();
						if (ImGui::CollapsingHeader("Loaded textures : ")) {
							for (int textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
								const Texture * texture = textures[textureIndex];
								std::stringstream ss;
								ss << texture->GetSize().x << " " << texture->GetSize().x << "\t" << texture->GetPath();
								ImGui::Text( ss.str().c_str() );
							}						
						}

						if (ImGui::CollapsingHeader("Rendered Models : ")) {
							for (int drawDataIndex = 0; drawDataIndex < m_drawData.size(); drawDataIndex++) {
								const DrawData & drawData = m_drawData[drawDataIndex];
								if (drawData.model != nullptr) {
									std::stringstream ss;
									ss << drawData.model->GetEntity()->GetName() << " " << drawData.meshData->mesh->GetPath();
									ImGui::Text(ss.str().c_str());
								}
								else {
									ImGui::Text("Empty slot");
								}
							}
						}
					}
				}
				if (m_ressourceManager->IsModified()) {
					ReloadShaders();
					m_ressourceManager->SetUnmodified();
				}

				ImGui::End();
				ImGui::EndFrame();
				ImGui::Render();

				const uint32_t currentFrame = m_swapchain->GetCurrentFrame();
				UpdateUniformBuffer();
				m_forwardPipeline->UpdateUniformBuffers( m_lightsUniform );
				if (m_reloadGeometryCommandBuffers[currentFrame] == true) {
					RecordCommandBufferGeometry(currentFrame);
				}
				RecordCommandBufferDebug(currentFrame);
				RecordCommandBufferImgui(currentFrame);
				RecordPrimaryCommandBuffer(currentFrame);
				SubmitCommandBuffers();

				m_swapchain->PresentImage();
				m_swapchain->StartNextFrame();
				Input::NewFrame();
				ImGui::NewFrame();
				ClearDebug();
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::WaitIdle() { 
			vkDeviceWaitIdle(m_device->vkDevice); 
		}

		//================================================================================================================================
		//================================================================================================================================
		VkCommandBuffer Renderer::BeginSingleTimeCommands()
		{
			// Allocate a temporary command buffer for memory transfer operations
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = m_commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(m_device->vkDevice, &allocInfo, &commandBuffer);

			// Start recording the command buffer
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			return commandBuffer;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
		{
			vkEndCommandBuffer(commandBuffer);

			// Execute the command buffer to complete the transfer
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(m_device->GetGraphicsQueue());

			// Cleaning
			vkFreeCommandBuffers(m_device->vkDevice, m_commandPool, 1, &commandBuffer);
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Renderer::ResetCommandPool() {
			VkCommandPoolResetFlags releaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;

			if (vkResetCommandPool(m_device->vkDevice, m_commandPool, releaseResources) != VK_SUCCESS) {
				Debug::Error( "Could not reset command pool." );
				return false;
			}
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::UpdateSceneUniforms() {
			// Force reload of transform uniforms
			for (int drawDataIndex = 0; drawDataIndex < m_drawData.size(); drawDataIndex++) {
				if (m_drawData[drawDataIndex].transform != nullptr ) {
				m_drawData[drawDataIndex].transform->MarkModified();
			}

				Material * material = m_drawData[drawDataIndex].material;
				if (material != nullptr) {
					material->MarkModified();
				}
			}
		}
		//================================================================================================================================
		//================================================================================================================================
		float  Renderer::GetWindowAspectRatio() const { 
			return static_cast<float>( m_swapchain->GetExtent().width ) / m_swapchain->GetExtent().height ;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position ) {
			m_camera.projection = _projection;
			m_camera.view		= _view;
			m_camera.position	= _position;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::SetDirectionalLight( const int _index, const glm::vec4 _direction, const glm::vec4 _ambiant, const glm::vec4 _diffuse, const glm::vec4 _specular ) {
			assert( _index  < s_maximumNumDirectionalLights );
			m_lightsUniform.dirLights[_index].direction = _direction;
			m_lightsUniform.dirLights[_index].ambiant = _ambiant;
			m_lightsUniform.dirLights[_index].diffuse = _diffuse;
			m_lightsUniform.dirLights[_index].specular = _specular;
			
		}

		//================================================================================================================================
		//================================================================================================================================
		void  Renderer::SetNumDirectionalLights( const uint32_t _num ) {
			assert( _num < s_maximumNumDirectionalLights );
			m_lightsUniform.dirLightsNum = _num;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::UpdateUniformBuffer()
		{
			// Main camera transform
			{			
				ForwardPipeline::VertUniforms ubo = m_forwardPipeline->GetVertUniforms();
				ubo.view = m_camera.view;
				ubo.proj = m_camera.projection;
				ubo.proj[1][1] *= -1;
				m_forwardPipeline->SetVertUniforms(ubo);

				ForwardPipeline::FragUniforms fragUniforms = m_forwardPipeline->GetFragUniforms();
				fragUniforms.cameraPosition = m_camera.position;
				m_forwardPipeline->SetFragUniforms(fragUniforms);

				DebugPipeline::Uniforms debugUniforms;
				debugUniforms.model = glm::mat4(1.0);
				debugUniforms.view = ubo.view;
				debugUniforms.proj = ubo.proj;
				debugUniforms.color = glm::vec4(1, 1, 1, 1);
				m_debugLinesPipeline->SetUniforms(debugUniforms);
				m_debugLinesPipelineNoDepthTest->SetUniforms(debugUniforms);
				m_debugTrianglesPipeline->SetUniforms(debugUniforms);
			}

			// Dynamic uniforms 
			bool mustUpdateDynamicUniformsVert = false;
			for (int drawDataIndex = 0; drawDataIndex < m_drawData.size() ; drawDataIndex++) {			
				DrawData & drawData = m_drawData[drawDataIndex];

				if (drawData.model != nullptr) {

					// Vert
					if (drawData.transform->IsModified() == true ) {
						mustUpdateDynamicUniformsVert = true;
						ForwardPipeline::DynamicUniformsVert uniform;
						uniform.modelMat = drawData.transform->GetModelMatrix();
						uniform.rotationMat = drawData.transform->GetRotationMat();
						m_forwardPipeline->SetDynamicUniformVert(uniform, drawDataIndex);
					}

					// Frag
					if (drawData.material != nullptr && (drawData.material->IsModified())) {
						Texture * texture = drawData.material->GetTexture();
						if (texture != nullptr) {
							m_mustUpdateDynamicUniformsFrag = true;
							ForwardPipeline::DynamicUniformsMaterial uniform;
							uniform.color = drawData.material->GetColor().ToGLM();
							uniform.textureIndex = texture->GetRenderID();
							uniform.shininess =    static_cast<uint32_t>(drawData.material->GetShininess());
							assert(uniform.textureIndex >= 0);
							m_forwardPipeline->SetDynamicUniformFrag(uniform, drawDataIndex);
						}
					}
				}
			}

			m_mustUpdateDynamicUniformsFrag = false;
		}
	
		//================================================================================================================================
		//================================================================================================================================
		void Renderer::RecordAllCommandBuffers() {
			for (int cmdBufferIndex = 0; cmdBufferIndex < m_imguiCommandBuffers.size(); cmdBufferIndex++) {
				RecordCommandBufferImgui(cmdBufferIndex);
			}
			for (int cmdBufferIndex = 0; cmdBufferIndex < m_geometryCommandBuffers.size(); cmdBufferIndex++) {
				RecordCommandBufferGeometry(cmdBufferIndex);
			}
			for (int cmdBufferIndex = 0; cmdBufferIndex < m_debugCommandBuffers.size(); cmdBufferIndex++) {
				RecordCommandBufferDebug(cmdBufferIndex);
			}
			for (int cmdBufferIndex = 0; cmdBufferIndex < m_swapchainFramebuffers.size(); cmdBufferIndex++) {
				RecordCommandBufferPostProcess( cmdBufferIndex );
			}

			for (int cmdBufferIndex = 0; cmdBufferIndex < m_primaryCommandBuffers.size(); cmdBufferIndex++) {
				RecordPrimaryCommandBuffer(cmdBufferIndex);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::RecordPrimaryCommandBuffer(const int _index) {
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
			renderPassInfo.framebuffer = m_forwardFrameBuffers[_index]->GetFrameBuffer();
			renderPassInfo.renderArea.offset = { 0,0 };
			renderPassInfo.renderArea.extent.width = m_swapchain->GetExtent().width;
			renderPassInfo.renderArea.extent.height = m_swapchain->GetExtent().height;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			VkRenderPassBeginInfo renderPassInfoPostprocess = {};
			renderPassInfoPostprocess.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfoPostprocess.pNext = nullptr;
			renderPassInfoPostprocess.renderPass = m_renderPassPostprocess;
			renderPassInfoPostprocess.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
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
		void Renderer::RecordCommandBufferPostProcess( const int _index ) {

			VkCommandBuffer commandBuffer = m_postprocessCommandBuffers[_index];

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
			commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
			//commandBufferInheritanceInfo.queryFlags				=;
			//commandBufferInheritanceInfo.pipelineStatistics		=;

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

			if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
				m_postprocessPipeline->Draw(commandBuffer);

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
		void Renderer::RecordCommandBufferImgui(const int _index) {

			m_imguiPipeline->UpdateBuffer(_index);

			VkCommandBuffer commandBuffer = m_imguiCommandBuffers[_index];

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
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
		void Renderer::RecordCommandBufferDebug(const int _index) {
			if (HasNoDebugToDraw() == false) {
				UpdateDebugBuffer(_index);

				VkCommandBuffer commandBuffer = m_debugCommandBuffers[_index];

				VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
				commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				commandBufferInheritanceInfo.pNext = nullptr;
				commandBufferInheritanceInfo.renderPass = m_renderPass;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = m_forwardFrameBuffers[_index]->GetFrameBuffer();
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags				=;
				//commandBufferInheritanceInfo.pipelineStatistics		=;

				VkCommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				commandBufferBeginInfo.pNext = nullptr;
				commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {				
					m_debugLinesPipeline->Draw(				commandBuffer, *m_debugLinesvertexBuffers[_index],				static_cast<uint32_t>(m_debugLines.size()));		
					m_debugLinesPipelineNoDepthTest->Draw(	commandBuffer, *m_debugLinesNoDepthTestVertexBuffers[_index],	static_cast<uint32_t>(m_debugLinesNoDepthTest.size()));
					m_debugTrianglesPipeline->Draw(			commandBuffer, *m_debugTrianglesvertexBuffers[_index],			static_cast<uint32_t>(m_debugTriangles.size()));
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
		void Renderer::RecordCommandBufferGeometry(const int _index) {

			VkCommandBuffer commandBuffer = m_geometryCommandBuffers[_index];

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = m_renderPass;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = m_forwardFrameBuffers[_index]->GetFrameBuffer();
			commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
			//commandBufferInheritanceInfo.queryFlags				=;
			//commandBufferInheritanceInfo.pipelineStatistics		=;

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

			if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {

				m_forwardPipeline->Draw(commandBuffer, m_drawData );
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

			m_postprocessPipeline->ReloadShaders();
			m_forwardPipeline->ReloadShaders();
			m_debugLinesPipeline->ReloadShaders();
			m_debugLinesPipelineNoDepthTest->ReloadShaders();
			m_debugTrianglesPipeline->ReloadShaders();

			m_postprocessPipeline->Resize(m_swapchain->GetExtent());
			m_forwardPipeline->Resize(m_swapchain->GetExtent());
			m_debugLinesPipeline->Resize(m_swapchain->GetExtent());
			m_debugLinesPipelineNoDepthTest->Resize(m_swapchain->GetExtent());
			m_debugTrianglesPipeline->Resize(m_swapchain->GetExtent());

			DeleteForwardFramebuffers();
			CreateForwardFramebuffers();
			RecordAllCommandBuffers();	

			UpdateSceneUniforms();
		}	

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::UpdateDebugBuffer(const int _index) {
			if( m_debugLines.size() > 0) {
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
					VkCommandBuffer cmd = Renderer::Get().BeginSingleTimeCommands();
					stagingBuffer.CopyBufferTo(cmd, m_debugLinesvertexBuffers[_index]->GetBuffer(), size);
					Renderer::Get().EndSingleTimeCommands(cmd);
				}
			}

			if (m_debugLinesNoDepthTest.size() > 0) {
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
					VkCommandBuffer cmd = Renderer::Get().BeginSingleTimeCommands();
					stagingBuffer.CopyBufferTo(cmd, m_debugLinesNoDepthTestVertexBuffers[_index]->GetBuffer(), size);
					Renderer::Get().EndSingleTimeCommands(cmd);
				}
			}

			if(m_debugTriangles.size() > 0 ){
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
					VkCommandBuffer cmd = Renderer::Get().BeginSingleTimeCommands();
					stagingBuffer.CopyBufferTo(cmd, m_debugTrianglesvertexBuffers[_index]->GetBuffer(), size);
					Renderer::Get().EndSingleTimeCommands(cmd);
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
		//================================================================================================================================
		void Renderer::DebugTriangle(const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color) {
			const glm::vec3 normal = glm::normalize(ToGLM((_v1 - _v2).cross(_v0 - _v2)));

			m_debugTriangles.push_back(DebugVertex(ToGLM(_v0), normal, _color.ToGLM()));
			m_debugTriangles.push_back(DebugVertex(ToGLM(_v1), normal, _color.ToGLM()));
			m_debugTriangles.push_back(DebugVertex(ToGLM(_v2), normal, _color.ToGLM()));
		}

		//================================================================================================================================
		//================================================================================================================================
		std::vector< btVector3> Renderer::DebugCube(const btTransform _transform, const float _halfSize, const Color _color) {
			std::vector< btVector3 > square = GetCube(_halfSize);		

			for (int vertIndex = 0; vertIndex < square.size(); vertIndex++)	{
				square[vertIndex] = _transform * square[vertIndex];
			}

			for (int triangleIndex = 0; triangleIndex < square.size() / 3; triangleIndex++) {
				DebugTriangle(square[3 * triangleIndex + 0], square[3 * triangleIndex + 1], square[3 * triangleIndex + 2], _color);
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
				DebugLine(v0, v1, _color);
				DebugLine(v1, v2, _color);
				DebugLine(v2, v0, _color);
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
		void Renderer::RegisterMaterial(Material * _material ) {
			Model * model = _material->GetEntity()->GetComponent<Model>();
			if (model != nullptr && model->GetRenderID() >= 0 ) {
				m_drawData[model->GetRenderID()].material = _material;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::UnRegisterMaterial(Material * _material) {
			Model * model = _material->GetEntity()->GetComponent<Model>();
			if (model != nullptr && model->GetRenderID() >= 0) {
				DrawData & drawData = m_drawData[model->GetRenderID()];
				drawData.material = nullptr;
				ForwardPipeline::DynamicUniformsMaterial uniform;
				uniform.textureIndex = 0;
				m_forwardPipeline->SetDynamicUniformFrag(uniform, model->GetRenderID());
				m_mustUpdateDynamicUniformsFrag = true;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::RegisterModel( Model * _model) {		
		
			DrawData * drawData = nullptr;

			// Looks for the model
			for (int modelIndex = 0; modelIndex < m_drawData.size() ; modelIndex++){
				if (m_drawData[modelIndex].model == _model) {
					drawData = &m_drawData[modelIndex];
					_model->SetRenderID(modelIndex);
					break;
				}
			}

			// no model found -> creates draw data for it
			if (drawData == nullptr) {
				// Looks for an empty index
				int emptyIndex = -1;
				for (int modelIndex = 0; modelIndex < m_drawData.size(); modelIndex++) {
					if (m_drawData[modelIndex].model == nullptr) {
						emptyIndex = modelIndex;
						break;
					}
				}

				// Fills in new data
				if (emptyIndex < 0) {
					m_drawData.push_back({});
					drawData = &m_drawData[m_drawData.size() - 1];
					_model->SetRenderID(static_cast<int>(m_drawData.size() - 1));
				} else {
					drawData = &m_drawData[emptyIndex];
					_model->SetRenderID(static_cast<int>(emptyIndex));
				}
			}

			drawData->model = _model;
			drawData->transform = _model->GetEntity()->GetComponent<Transform>();
			drawData->material =  _model->GetEntity()->GetComponent<Material>();
			drawData->meshData = m_ressourceManager->FindMeshData(_model->GetMesh());
			drawData->transform->MarkModified(); // Force tranform uniforms update

			for (int boolIndex = 0; boolIndex < m_reloadGeometryCommandBuffers.size(); boolIndex++) {
				m_reloadGeometryCommandBuffers[boolIndex] = true;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::UnRegisterModel(Model * _model) {
			for (int modelIndex = 0; modelIndex < m_drawData.size(); modelIndex++) {
				if (m_drawData[modelIndex].model == _model) {
					m_drawData[modelIndex] = {};
				}
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::Clear() {
			for (int modelIndex = 0; modelIndex < m_drawData.size(); modelIndex++) {
				m_drawData[modelIndex] = {};				
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Renderer::CreateCommandBuffers() {
			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = m_commandPool;
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
			secondaryCommandBufferAllocateInfo.commandPool = m_commandPool;
			secondaryCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			secondaryCommandBufferAllocateInfo.commandBufferCount = m_swapchain->GetSwapchainImagesCount();

			m_reloadGeometryCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount(), false);
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

			m_postprocessCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
			if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_postprocessCommandBuffers.data()) != VK_SUCCESS) {
				Debug::Error( "Could not allocate command buffers." );
				return false;
			}

			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Renderer::CreateCommandPool() {
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = nullptr;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = m_device->GetGraphicsQueueFamilyIndex();

			if (vkCreateCommandPool(m_device->vkDevice, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
				Debug::Error( "Could not allocate command pool." );
				return false;
			}
			Debug::Get() << Debug::Severity::log << std::hex << "VkCommandPool         " << m_commandPool << std::dec << Debug::Endl();
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
		bool Renderer::CreateRenderPassUI() {
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
			m_forwardFrameBuffers.resize(m_swapchain->GetSwapchainImagesCount());
			for (int framebufferIndex = 0; framebufferIndex < m_forwardFrameBuffers.size(); framebufferIndex++) {
				std::vector<VkImageView> attachments =
				{
					m_postprocessPipeline->GetImageView(),			
					m_forwardPipeline->GetDepthImageView()
				};

				m_forwardFrameBuffers[framebufferIndex] = new FrameBuffer(*m_device);
				m_forwardFrameBuffers[framebufferIndex]->Create(m_renderPass, attachments, m_swapchain->GetExtent());
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::CreateSwapchainFramebuffers() {		

			m_swapchainFramebuffers.resize(m_swapchain->GetSwapchainImagesCount());
			for (int framebufferIndex = 0; framebufferIndex < m_swapchainFramebuffers.size(); framebufferIndex++) {
				std::vector<VkImageView> attachments =
				{
					m_swapchain->GetImageView(framebufferIndex),
				};

				m_swapchainFramebuffers[framebufferIndex] = new FrameBuffer(*m_device);
				m_swapchainFramebuffers[framebufferIndex]->Create( m_renderPassPostprocess, attachments, m_swapchain->GetExtent());

			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Renderer::DeleteSwapchainFramebuffers() {
			for (int framebufferIndex = 0; framebufferIndex < m_swapchainFramebuffers.size(); framebufferIndex++) {
				delete m_swapchainFramebuffers[framebufferIndex];
			}
			m_swapchainFramebuffers.clear();
		}	
	   	
		//================================================================================================================================
		//================================================================================================================================
		void Renderer::DeleteCommandPool() {
			vkDestroyCommandPool(m_device->vkDevice, m_commandPool, nullptr);
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
		void Renderer::DeleteForwardFramebuffers() {
			for (int framebufferIndex = 0; framebufferIndex < m_forwardFrameBuffers.size(); framebufferIndex++) {
				delete m_forwardFrameBuffers[framebufferIndex];
			}
			m_forwardFrameBuffers.clear();
		}	
}