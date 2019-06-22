#include "vkRenderer.h"

#include "util/Time.h"
#include "util/glfwInput.h"

namespace vk {

	Renderer * Renderer::ms_globalRenderer = nullptr;

	void Renderer::Run()
	{
		ImGuiIO& io = ImGui::GetIO();
		float lastUpdateTime = Time::ElapsedSinceStartup();

		while ( glfwWindowShouldClose(m_window->GetWindow()) == false)
		{
			vkWaitForFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
			vkResetFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence());

			VkResult result = m_swapchain->AcquireNextImage();
			(void)result;
			/*if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				std::cout << "suboptimal swapchain" << std::endl;
				vkDeviceWaitIdle(m_device->vkDevice);

				DeletePipeline();
				DeleteFramebuffers();
				DeleteDepthRessources();
				DeleteRenderPass();
				DeleteCommandBuffers();
				DeleteCommandPool();
				DeleteDescriptors();

				SwapChain * newSwapchain = new SwapChain(m_device, m_window->GetSurface(), m_window->GetFramebufferSize(), m_swapchain->GetVkSwapChain());
				delete m_swapchain;
				m_swapchain = newSwapchain;

				CreateCommandPool();
				CreateRenderPass();
				CreateDepthRessources();
				CreateFramebuffers();
				CreateCommandBuffers();
				CreateDescriptors();
				CreatePipeline();
				RecordCommandBuffers();

				m_swapchain->AcquireNextImage();
			}
			else if (result != VK_SUCCESS) {
				std::cout << "Could not acquire next image" << std::endl;
			}*/
			
			const float time = Time::ElapsedSinceStartup();
			const float delta = time - lastUpdateTime;
			lastUpdateTime = time;
			io.DisplaySize = ImVec2(static_cast<float>(m_swapchain->GetExtent().width), static_cast<float>(m_swapchain->GetExtent().height));

			Input::NewFrame();
			ImGui::NewFrame();

			UpdateUniformBuffer();
			ImGui::Render();
			ImGui::EndFrame();
			m_imguiPipeline->UpdateBuffers();
			RecordCommandBuffer( m_swapchain->GetCurrentFrame() );
			RecordCommandBuffersImgui(m_swapchain->GetCurrentFrame());

			SubmitCommandBuffers();			

			m_swapchain->PresentImage();
			m_swapchain->StartNextFrame();
		}
	}

	void Renderer::CreateVertexBuffers() {

		delete m_indexBuffer;
		delete m_vertexBuffer;

		{
			m_indices = { //cube
				 0,1,2	,1,3,2	// top
				,6,5,4	,7,5,6	// bot
				,7,6,2	,7,2,3	
				,6,4,0	,6,0,2
				,4,5,0	,5,1,0
				,7,1,5	,7,3,1
			};			

			const VkDeviceSize size = sizeof(m_indices[0]) * m_indices.size();

			m_indexBuffer = new Buffer(m_device);
			m_indexBuffer->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			Buffer stagingBuffer(m_device);
			stagingBuffer.Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer.SetData(m_indices.data(), size);
			VkCommandBuffer cmd = BeginSingleTimeCommands();
			stagingBuffer.CopyBufferTo(cmd, m_indexBuffer->GetBuffer(), size);
			EndSingleTimeCommands(cmd);
		}
		{
			glm::vec3 color(0.2, 0.2, 0.2);

			Vertex v0 = { { +0.5,+0.5,+0.5},	color,{} };
			Vertex v1 = { { +0.5,+0.5,-0.5},	color,{} };
			Vertex v2 = { { -0.5,+0.5,+0.5},	color,{} };
			Vertex v3 = { { -0.5,+0.5,-0.5},	color,{} };
			Vertex v4 = { { +0.5,-0.5,+0.5},	color,{} };
			Vertex v5 = { { +0.5,-0.5,-0.5},	color,{} };
			Vertex v6 = { { -0.5,-0.5,+0.5},	color,{} };
			Vertex v7 = { { -0.5,-0.5,-0.5},	color,{} };
			m_vertices = { v0, v1 ,v2 ,v3 ,v4 ,v5 ,v6 ,v7 };

			const VkDeviceSize size = sizeof(m_vertices[0]) * m_vertices.size();
			m_vertexBuffer = new Buffer(m_device);
			m_vertexBuffer->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			Buffer stagingBuffer2(m_device);
			stagingBuffer2.Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer2.SetData(m_vertices.data(), size);
			VkCommandBuffer cmd2 = BeginSingleTimeCommands();
			stagingBuffer2.CopyBufferTo(cmd2, m_vertexBuffer->GetBuffer(), size);
			EndSingleTimeCommands(cmd2);
		}
	}

	void Renderer::UpdateUniformBuffer()
	{
		// Ui
		static float s_speed = 1.f;
		ImGui::Begin("UniformBuffer");
		{
			ImGui::SliderFloat("rotation speed", &s_speed, 0.f, 1000.f);
		}
		ImGui::End();

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), Time::ElapsedSinceStartup() * glm::radians(s_speed), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->GetExtent().width / (float)m_swapchain->GetExtent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1; 			//the Y coordinate of the clip coordinates is inverted

		m_uniformBuffer->SetData(&ubo, sizeof(ubo));
	}
}