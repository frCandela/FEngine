#include "Renderer.h"
#include "VulkanInitializers.hpp"

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
	texture->LoadTexture("textures/cube.jpg");

	textureSampler = new vk::Sampler(*device, texture->m_mipLevels);
	descriptors = new vk::Descriptors(*device);

	descriptors->updateUniformBuffers(*m_pCamera);
	descriptors->updateDynamicUniformBuffer(true);

	createGraphicsPipeline();

	vk::Buffer * cube = new vk::Buffer(*device);
	cube->LoadModel("mesh/cube.obj");
	buffers.push_back(cube);

	vk::Buffer * sphere = new vk::Buffer(*device);
	sphere->LoadModel("mesh/sphere.obj");
	buffers.push_back(sphere);


	descriptors->createDescriptorSet(*texture, *textureSampler);
	createCommandBuffers();
	createSyncObjects();
}

// Setup the command buffers for drawing opérations
void Renderer::createCommandBuffers()
{
	commands->commandBuffers.resize(swapChain->swapChainFramebuffers.size());

	// VkCommandBufferAllocateInfo specifies the command pool and number of buffers to allocate
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commands->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commands->commandBuffers.size();

	if (vkAllocateCommandBuffers(device->device, &allocInfo, commands->commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");

	// Records every command buffer (one per framebuffer)
	for (size_t i = 0; i < commands->commandBuffers.size(); i++)
	{
		// Specify the usage of the command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commands->commandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		// Configure the render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->renderPass;
		renderPassInfo.framebuffer = swapChain->swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->swapChainExtent;

		//Set clear collors for color and depth attachments
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commands->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		//Record Draw calls on all existing buffers
		for (int j = 0; j < buffers.size(); ++j)
		{
			VkBuffer vertexBuffers[] = { buffers[j]->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commands->commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commands->commandBuffers[i], buffers[j]->indexBuffer, 0, VK_INDEX_TYPE_UINT32);


			// One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices
			uint32_t dynamicOffset = j * static_cast<uint32_t>(descriptors->dynamicAlignment);
			// Bind the descriptor set for rendering a mesh using the dynamic offset
			vkCmdBindDescriptorSets(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptors->descriptorSet, 1, &dynamicOffset);

			vkCmdDrawIndexed(commands->commandBuffers[i], static_cast<uint32_t>(buffers[j]->indices.size()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commands->commandBuffers[i]);

		if (vkEndCommandBuffer(commands->commandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!"); 
	}
	/*commands->commandBuffers.resize(swapChain->swapChainFramebuffers.size());

	// VkCommandBufferAllocateInfo specifies the command pool and number of buffers to allocate
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commands->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commands->commandBuffers.size();

	if (vkAllocateCommandBuffers(device->device, &allocInfo, commands->commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");

	// Records every command buffer (one per framebuffer)
	for (size_t i = 0; i < commands->commandBuffers.size(); i++)
	{
		// Specify the usage of the command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commands->commandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		// Configure the render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->renderPass;
		renderPassInfo.framebuffer = swapChain->swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain->swapChainExtent;

		//Set clear collors for color and depth attachments
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commands->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		//Record Draw calls on all existing buffers
		for (int j = 0; j < buffers.size(); ++j)
		{
			

			VkBuffer vertexBuffers[] = { buffers[j]->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commands->commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commands->commandBuffers[i], buffers[j]->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			
			uint32_t dynamicOffset = j * static_cast<uint32_t>(descriptors->dynamicAlignment);
			vkCmdBindDescriptorSets(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptors->descriptorSet, 1, &dynamicOffset);
			vkCmdDrawIndexed(commands->commandBuffers[i], static_cast<uint32_t>(buffers[j]->indices.size()), 1, 0, 0, 0);

			VkBuffer vertexBuffers[] = { buffers[j]->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commands->commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commands->commandBuffers[i], buffers[j]->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commands->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(descriptors->descriptorSet), 0, nullptr);
			vkCmdDrawIndexed(commands->commandBuffers[i], static_cast<uint32_t>(buffers[j]->indices.size()), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commands->commandBuffers[i]);

		if (vkEndCommandBuffer(commands->commandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");
	}*/
}

Renderer::~Renderer()
{
	vkDeviceWaitIdle(device->device);
	cleanup();
}

// Draw a frame
void Renderer::drawFrame()
{
	/*float aspectRatio = swapChain->swapChainExtent.width / (float)swapChain->swapChainExtent.height;
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	//Define the model, view and projection transformations in the uniform buffer object 
	vk::UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = m_pCamera->GetView();
	ubo.proj = m_pCamera->GetProj();*/

	descriptors->updateDynamicUniformBuffer();

	/*glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 model2 = glm::rotate(glm::mat4(1.0f),  - time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	descriptors->UpdateModelBuffer(model, model2);*/

	//std::numeric_limits<uint64_t>::max()
	vkWaitForFences(device->device, 1, &inFlightFences[currentFrame], VK_TRUE, 0xFFFFFFFFFFFFFFFF);
	vkResetFences(device->device, 1, &inFlightFences[currentFrame]);

	//Acquire an image from the swap chain
	uint32_t imageIndex;
	
		//std::numeric_limits<uint64_t>::max()
	VkResult result = vkAcquireNextImageKHR(device->device, swapChain->swapChain, 0xFFFFFFFFFFFFFFFF, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//Suboptimal or out-of-date swap chain
	if (result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		throw std::runtime_error("failed to acquire swap chain image!");	

	//Submit info struct
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//Specify which semaphores to wait on before execution begins and in which stages of the pipeline to wait.
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(commands->commandBuffers[imageIndex]);

	//Specify which semaphores to signal once the command buffers have finished execution
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//Submit draw command buffer
	if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) 
		throw std::runtime_error("failed to submit draw command buffer!");
	
	//Specify which semaphores to wait on before presentation can happen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	// swap chains to present images to
	VkSwapchainKHR swapChains[] = { swapChain->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	//Submit the result back to the swap chain to have it on screen
	result = vkQueuePresentKHR(device->presentQueue, &presentInfo);

	//Suboptimal or out-of-date swap chain
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
	
	for(vk::Buffer* buffer : buffers)
		delete(buffer);

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