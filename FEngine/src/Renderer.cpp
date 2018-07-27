#include "Renderer.h"

#include <array>
#include <chrono>
#include "glm/gtc/matrix_transform.hpp"

Renderer::Renderer(Window& rWindow, Camera& rCamera) :
	m_window(rWindow),
	m_pCamera(&rCamera)
{
	// Initializes the Vulkan application and required components
	instance = new vk::Instance();
	device = new vk::Device(instance->instance, m_window);
	commands = new vk::Commands(*device);
	device->commands = commands;	//zob

	swapChain = new vk::SwapChain(*device);
	swapChain->BuildSwapChain(m_window);

	vertShader = new vk::Shader(*device, "shaders/vert.spv");
	fragShader = new vk::Shader(*device, "shaders/frag.spv");
	renderPass = new vk::RenderPass(*device, *swapChain);
	swapChain->CreateFramebuffers(renderPass->renderPass);

	texture = new vk::Texture(*device);
	texture->LoadTexture("textures/texture.jpg");

	textureSampler = new vk::Sampler(*device, texture->m_mipLevels);
	descriptors = new vk::Descriptors(*device);

	descriptors->UpdateUniformBuffers(*m_pCamera);
	descriptors->UpdateDynamicUniformBuffer({glm::mat4(1.f), glm::mat4(1.f) });

	createGraphicsPipeline();

	vk::Mesh * cube = new vk::Mesh(*device);
	cube->LoadModel("mesh/cube.obj");
	cube->CreateBuffers();
	buffers.push_back(cube);

	vk::Mesh * sphere = new vk::Mesh(*device);
	sphere->LoadModel("mesh/sphere.obj");

	glm::vec2 bl = { 0,0 };
	glm::vec2 br = { 1,0 };
	glm::vec2 tl = { 0,1 };//vert
	glm::vec2 tr = { 1,1 };//jaune

	glm::vec3 red =		{ 1,0,0 };
	glm::vec3 green =	{ 0,1,0 };
	glm::vec3 blue =	{ 0,0,1 };
	glm::vec3 white =	{ 1,1,1 };
	glm::vec3 yellow =	{ 1,1,0 };
	glm::vec3 pink =	{ 1,0,1 };
	glm::vec3 cyan =	{ 0,1,1 };
	glm::vec3 black =	{ 1,1,1 };
	
	sphere->indices = { 
		0,1,3,0,3,2, //front
		4,7,5,4,6,7, //back
		1,5,7,1,7,3, //right
		0,6,4,0,2,6, //left
		2,3,7,2,7,6, //top
		0,5,1,0,4,5  //
	};

	sphere->vertices = 
	{
		{ {0,0,0},	red,	{0,0} },	//Fbl 0
		{ {0,0,1},	white,	{1,0} },	//Fbr 1
		{ {0,1,0},	blue,	{0,1} },	//Ftl 2
		{ {0,1,1},	green,	{1,1 } },	//Ftr 3
		{ {1,0,0},	yellow,	{0,1} },	//Bbl 4
		{ {1,0,1},	pink,	{1,1} },	//Bbr 5
		{ {1,1,0},	cyan,	{0,0} },	//Btl 6
		{ {1,1,1},	black,	{1,0} },	//Btr 7///
	};

	sphere->CreateBuffers();
	buffers.push_back(sphere);

	descriptors->CreateDescriptorSet(*texture, *textureSampler);
	glm::vec2 size = GetSize();
	ImGui::CreateContext();
	imGui = new ImGUI(device);
	imGui->camera.type = Kamera::CameraType::lookat;
	imGui->camera.setPosition(glm::vec3(0.0f, 1.4f, -4.8f));
	imGui->camera.setRotation(glm::vec3(4.5f, -380.0f, 0.0f));
	imGui->camera.setPerspective(45.0f, size.x / size.y, 0.1f, 256.0f);
	imGui->init(size.x, size.y, m_window.GetGLFWwindow());
	imGui->initResources( renderPass->renderPass, device->graphicsQueue);
	createCommandBuffers();
	createSyncObjects();

	//ImGui::NewFrame();
}

Renderer::~Renderer()
{
	vkDeviceWaitIdle(device->device);
	cleanup();
}

// Setup the command buffers for drawing opérations
void Renderer::createCommandBuffers()
{
	commands->CreateBuffer(swapChain->swapChainFramebuffers.size());

	imGui->updateBuffers();

	// Records every command buffer (one per framebuffer)
	for (size_t i = 0; i < commands->commandBuffers.size(); i++)
	{
		commands->Begin(i);

		// Configure the render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->renderPass;
		renderPassInfo.framebuffer = swapChain->swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->swapChainExtent;

		//Set clear collors for color and depth attachments
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.5f, 0.5f, 0.5f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
		
		vkCmdBeginRenderPass(commands->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);		
		vkCmdBindPipeline(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		
		//Record Draw calls on all existing buffers
		for (int j = 0; j < buffers.size(); ++j)
		{
			VkBuffer vertexBuffers[] = { buffers[j]->vertexBuffer.m_buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commands->commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commands->commandBuffers[i], buffers[j]->indexBuffer.m_buffer, 0, VK_INDEX_TYPE_UINT32);

			// One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices
			uint32_t dynamicOffset = j * static_cast<uint32_t>(descriptors->dynamicAlignment);
		
			// Bind the descriptor set for rendering a mesh using the dynamic offset
			vkCmdBindDescriptorSets(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptors->descriptorSet, 1, &dynamicOffset);
			vkCmdDrawIndexed(commands->commandBuffers[i], static_cast<uint32_t>(buffers[j]->indices.size()), 1, 0, 0, 0);
		}

		imGui->drawFrame(commands->commandBuffers[i]);

		vkCmdEndRenderPass(commands->commandBuffers[i]);

		commands->End(i);
	}
}

// Draw a frame
void Renderer::drawFrame()
{
	float aspectRatio = swapChain->swapChainExtent.width / (float)swapChain->swapChainExtent.height;
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	//time = 0.f;
	descriptors->UpdateDynamicUniformBuffer({
		glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.f, 1.f, 1.f))
		,glm::rotate(glm::mat4(1.0f), -time * glm::radians(90.0f), glm::vec3(1.f, 1.f, 1.f))
	});	
	
	vkWaitForFences(device->device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(device->device, 1, &inFlightFences[currentFrame]);
	
	vkDeviceWaitIdle(device->device);//zob
	
	ImGui::Render();
	createCommandBuffers();
	//ImGui::NewFrame();

	// Acquire an image from the swap chain
	uint32_t imageIndex;
	
	VkResult result = vkAcquireNextImageKHR(device->device, swapChain->swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	// Suboptimal or out-of-date swap chain
	if (result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		throw std::runtime_error("failed to acquire swap chain image!");	

	// Submit info struct
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	std::vector<VkCommandBuffer> commandBuffers = 
	{ 
		commands->commandBuffers[imageIndex]		
	};

	// Specify which semaphores to wait on before execution begins and in which stages of the pipeline to wait.
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	submitInfo.pCommandBuffers = commandBuffers.data();

	// Specify which semaphores to signal once the command buffers have finished execution
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	// Submit draw command buffer
	if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
		throw std::runtime_error("failed to submit draw command buffer!");
	
	// Specify which semaphores to wait on before presentation can happen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	// Swap chains to present images to
	VkSwapchainKHR swapChains[] = { swapChain->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	// Submit the result back to the swap chain to have it on screen
	result = vkQueuePresentKHR(device->presentQueue, &presentInfo);

	// Suboptimal or out-of-date swap chain
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
		recreateSwapChain();	
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to  swap chain image!");
	
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// Recreates the swap chain (necessary when the window is resized)
void Renderer::recreateSwapChain()
{
	vkDeviceWaitIdle(device->device);

	//Cleanup	
	commands->cleanup();
	vkDestroyPipeline(device->device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device->device, pipelineLayout, nullptr);
	delete(renderPass);

	swapChain->CleanupSwapChain();
	swapChain->BuildSwapChain(m_window);

	renderPass = new vk::RenderPass(*device, *swapChain);
	
	createGraphicsPipeline();
	
	swapChain->CreateFramebuffers(renderPass->renderPass);
	createCommandBuffers();
}

// Creates the graphics pipeline
void Renderer::createGraphicsPipeline()
{
	vk::ShaderModule vertShaderModule = vertShader->GetShaderModule();
	vk::ShaderModule fragShaderModule = fragShader->GetShaderModule();

	// Link vertex shader
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule.module;
	vertShaderStageInfo.pName = "main";

	// Link fragment shader
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule.module;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Get vertex input info
	auto bindingDescription = vk::Vertex::getBindingDescription();
	auto attributeDescriptions = vk::Vertex::getAttributeDescriptions();

	// Set vertex input info
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChain->swapChainExtent.width;
	viewport.height = (float)swapChain->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Scissor
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain->swapChainExtent;

	// References all the Viewports and Scissors
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// Rasterizer (also performs depth testing, face culling and scissor test)
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	// Multisampling (used for antialiasing)
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Configure the depth and stencil buffers
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	// Color blending atachemennt (contains the configuration per attached framebuffer)
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	// Color blending (contains the global color blending settings)
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	// Dynamic States (states that can be changed without recreating the pipeline)
	VkDynamicState dynamicStates[] = 
	{ 
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	// Dynamic States struct
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	//Pipeline layout for setting up uniforms in shaders
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = & (descriptors->descriptorSetLayout);

	if (vkCreatePipelineLayout(device->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout!");	

	//Creates the Graphics Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline!");	
}

// Creates the sync objects (fences and semaphores)
void Renderer::createSyncObjects()
{
	//Fences perform CPU-GPU synchronization to prevent more than MAX_FRAMES_IN_FLIGHT from being submitted

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	//Semaphores info
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fences info
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Creates semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
	{
		if (vkCreateSemaphore(device->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device->device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

// Clean Vulkan objects 
void Renderer::cleanup()
{
	vkDestroyPipeline(device->device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device->device, pipelineLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
	{
		vkDestroySemaphore(device->device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device->device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device->device, inFlightFences[i], nullptr);
	}	
	
	for(vk::Mesh* buffer : buffers)
		delete(buffer);

	delete(imGui);

	delete(descriptors);
	delete(textureSampler);
	delete(texture);
	delete(renderPass);
	delete(fragShader);
	delete(vertShader);
	delete(swapChain);
	delete(commands);
	delete (device);
	delete(instance);	
}