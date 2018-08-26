#include "renderer/ForwardPipeline.h"
#include "vulkan/Initializers.h"

ForwardPipeline::ForwardPipeline(vk::Device& device, vk::Texture& texture, vk::Sampler& sampler) :
	m_device(device)
	, view(device)
	, dynamic(device)
{
	CreateDescriptorSetLayout();
	CreateUniformBuffer();

	vertShader = new vk::Shader(m_device, "shaders/vert.spv");
	fragShader = new vk::Shader(m_device, "shaders/frag.spv");

	CreateDescriptorPool();
	CreateDescriptorSet(texture, sampler, descriptorPool);
}

ForwardPipeline::~ForwardPipeline()
{
	delete(fragShader);
	delete(vertShader);

	vkDestroyPipeline(m_device.device, graphicsPipeline1, nullptr);
	vkDestroyPipelineLayout(m_device.device, pipelineLayout1, nullptr);
	vkDestroyDescriptorPool(m_device.device, descriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(m_device.device, descriptorSetLayout, nullptr);
}

void ForwardPipeline::RenderGui()
{
	if (ImGui::CollapsingHeader("Forward Pipeline"))
		ImGui::DragFloat("Ambiant light", &uboRendererData.ambiant, 0.025, 0.f, 1.f);
}

void ForwardPipeline::BindPipeline(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline1);
}

void ForwardPipeline::BindDescriptors(VkCommandBuffer commandBuffer, int offsetIndex)
{
	// One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices
	uint32_t dynamicOffset = offsetIndex * static_cast<uint32_t>(dynamicAlignment);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout1, 0, 1, &descriptorSet, 1, &dynamicOffset);
}

void ForwardPipeline::CreateDescriptorSetLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
	{
		vk::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
		vk::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1),
		vk::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2)
	};

	VkDescriptorSetLayoutCreateInfo descriptorLayout =
		vk::init::descriptorSetLayoutCreateInfo(
			setLayoutBindings.data(),
			static_cast<uint32_t>(setLayoutBindings.size()));

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device.device, &descriptorLayout, nullptr, &descriptorSetLayout));
}

void ForwardPipeline::CreateUniformBuffer()
{
	// Calculate required alignment based on minimum device offset alignment
	size_t minUboAlignment = m_device.properties.limits.minUniformBufferOffsetAlignment;
	dynamicAlignment = sizeof(glm::mat4);
	if (minUboAlignment > 0) {
		dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
	}

	size_t bufferSize = OBJECT_INSTANCES * dynamicAlignment;

	uboDataDynamic.model = (glm::mat4*)_aligned_malloc(bufferSize, dynamicAlignment);
	assert(uboDataDynamic.model);

	// Static shared uniform buffer object with projection and view matrix
	view.CreateBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(uboRendererData)
	);

	// Uniform buffer object with per-object matrices
	dynamic.CreateBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		bufferSize
	);

	// Map persistent
	VK_CHECK_RESULT(view.Map());
	VK_CHECK_RESULT(dynamic.Map());
}

void ForwardPipeline::CreateDescriptorSet(vk::Texture& textureImage, vk::Sampler& textureSampler, VkDescriptorPool descriptorPool)
{
	VkDescriptorSetAllocateInfo allocInfo = vk::init::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);

	VK_CHECK_RESULT(vkAllocateDescriptorSets(m_device.device, &allocInfo, &descriptorSet));

	// Create image descriptor
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = textureImage.imageView;
	imageInfo.sampler = textureSampler.sampler;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 2;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	std::vector<VkWriteDescriptorSet> writeDescriptorSets =
	{
		// Binding 0 : Projection/View matrix uniform buffer			
		vk::init::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &view.descriptor)

		// Binding 1 : Instance matrix as dynamic uniform buffer
		,vk::init::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &dynamic.descriptor)

		// Binding 1 : Image sampler and texture
		,descriptorWrite

	};

	vkUpdateDescriptorSets(m_device.device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
}

void ForwardPipeline::UpdateUniforms(glm::mat4 projectionMat, glm::mat4 viewMat)
{
	// Fixed ubo with projection and view matrices
	uboRendererData.projection = projectionMat;
	uboRendererData.view = viewMat;

	memcpy(view.mappedData, &uboRendererData, sizeof(uboRendererData));
}

void ForwardPipeline::UpdateDynamicUniformBuffer(std::vector<glm::mat4> matrices)
{
	assert(matrices.size() <= OBJECT_INSTANCES);

	for (int i = 0; i < matrices.size(); ++i)
	{
		glm::mat4* modelMat = (glm::mat4*)(((uint64_t)uboDataDynamic.model + (i * dynamicAlignment)));
		*modelMat = matrices[i];
	}

	memcpy(dynamic.mappedData, uboDataDynamic.model, dynamic.m_size);

	// Flush to make changes visible to the host 
	VkMappedMemoryRange memoryRange = vk::init::mappedMemoryRange();
	memoryRange.memory = dynamic.memory;
	memoryRange.size = dynamic.m_size;
	vkFlushMappedMemoryRanges(m_device.device, 1, &memoryRange);
}

void ForwardPipeline::CreateGraphicsPipeline(VkRenderPass renderPass, VkExtent2D extent2D)
{
	// Destroy before create
	vkDestroyPipeline(m_device.device, graphicsPipeline1, nullptr);
	vkDestroyPipelineLayout(m_device.device, pipelineLayout1, nullptr);

	// Link vertex shader
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShader->shaderModule;
	vertShaderStageInfo.pName = "main";

	// Link fragment shader
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShader->shaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Get vertex input info
	auto bindingDescription = ForwardPipeline::Vertex::getBindingDescription();
	auto attributeDescriptions = ForwardPipeline::Vertex::getAttributeDescriptions();

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
	viewport.width = (float)extent2D.width;
	viewport.height = (float)extent2D.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Scissor
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent2D;

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
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(m_device.device, &pipelineLayoutInfo, nullptr, &pipelineLayout1) != VK_SUCCESS)
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
	pipelineInfo.layout = pipelineLayout1;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(m_device.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline1) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline!");
}

void ForwardPipeline::CreateDescriptorPool()
{
	VkDescriptorPoolSize descriptorPoolSizeUniform = {};
	descriptorPoolSizeUniform.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSizeUniform.descriptorCount = 1;

	VkDescriptorPoolSize descriptorPoolSizeUniformDynamic = {};
	descriptorPoolSizeUniformDynamic.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorPoolSizeUniformDynamic.descriptorCount = 1;

	VkDescriptorPoolSize descriptorPoolSizeImageSampler = {};
	descriptorPoolSizeImageSampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSizeImageSampler.descriptorCount = 1;

	// Example uses one ubo and one image sampler
	std::vector<VkDescriptorPoolSize> poolSizes =
	{
		descriptorPoolSizeUniform
		, descriptorPoolSizeUniformDynamic
		, descriptorPoolSizeImageSampler
	};

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(m_device.device, &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor pool!");
}

