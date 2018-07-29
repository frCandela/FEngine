#include "DebugPipeline.h"

DebugPipeline::DebugPipeline(vk::Device& rDevice) :
	 m_rDevice( rDevice )

{
	m_vertShaderDebug = new vk::Shader(m_rDevice, "shaders/debug/vert.spv");
	m_fragShaderDebug = new vk::Shader(m_rDevice, "shaders/debug/frag.spv");

	CreateDescriptorPool();
	CreateDescriptors();
}

DebugPipeline::~DebugPipeline()
{
	vkDestroyPipeline(m_rDevice.device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_rDevice.device, m_pipelineLayout, nullptr);
	vkDestroyDescriptorPool(m_rDevice.device, m_descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(m_rDevice.device, m_descriptorSetLayout, nullptr);

	delete(m_projViewBuffer);
	delete(m_fragShaderDebug);
	delete(m_vertShaderDebug);
}

void DebugPipeline::CreateGraphicsPipeline(VkRenderPass renderPass, VkExtent2D extent2D)
{
	vkDestroyPipeline(m_rDevice.device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_rDevice.device, m_pipelineLayout, nullptr);
	m_graphicsPipeline = VK_NULL_HANDLE;
	m_pipelineLayout = VK_NULL_HANDLE;

	// Link vertex shader
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = m_vertShaderDebug->shaderModule;
	vertShaderStageInfo.pName = "main";

	// Link fragment shader
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = m_fragShaderDebug->shaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Get vertex input info
	auto bindingDescription = DebugPipeline::Vertex::GetBindingDescription();
	auto attributeDescriptions = DebugPipeline::Vertex::GetAttributeDescriptions();

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
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
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
	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	// Dynamic States struct
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//Pipeline layout for setting up uniforms in shaders
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;

	if (vkCreatePipelineLayout(m_rDevice.device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
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
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(m_rDevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline!");
}

void DebugPipeline::Bind( VkCommandBuffer commandBuffer )
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, NULL);
	vkCmdSetLineWidth(commandBuffer, m_lineWidth);
}

void DebugPipeline::UpdateUniforms(glm::mat4 projection, glm::mat4 view)
{
	// Fixed ubo with projection and view matrices
	projView.projection = projection;
	projView.view = view;

	memcpy(m_projViewBuffer->mappedData, &projView, sizeof(projView));
}

void DebugPipeline::CreateDescriptorPool()
{
	VkDescriptorPoolSize descriptorPoolSizeUniform2 = {};
	descriptorPoolSizeUniform2.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSizeUniform2.descriptorCount = 1;

	// Example uses one ubo and one image sampler
	std::vector<VkDescriptorPoolSize> poolSizes = { descriptorPoolSizeUniform2 };

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(m_rDevice.device, &descriptorPoolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create descriptor pool!");
}

void DebugPipeline::CreateDescriptors()
{
	// Descriptor set layout
	VkDescriptorSetLayoutBinding setLayoutBinding = {};
	setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	setLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	setLayoutBinding.binding = 0;
	setLayoutBinding.descriptorCount = 1;

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = { setLayoutBinding };

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();
	descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_rDevice.device, &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout));

	// Static shared uniform buffer object with projection and view matrix
	m_projViewBuffer = new vk::Buffer(m_rDevice);
	m_projViewBuffer->CreateBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		sizeof(projView)
	);

	VK_CHECK_RESULT(m_projViewBuffer->Map());// Map persistent

										   // CreateDescriptorSet
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.pSetLayouts = &m_descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(m_rDevice.device, &allocInfo, &m_descriptorSet))

		// Binding 0 : Projection/View matrix uniform buffer
		VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = m_descriptorSet;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.pBufferInfo = &m_projViewBuffer->descriptor;
	writeDescriptorSet.descriptorCount = 1;

	// Create image descriptor
	std::vector<VkWriteDescriptorSet> writeDescriptorSets = { writeDescriptorSet };

	vkUpdateDescriptorSets(m_rDevice.device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
}

void DebugPipeline::RenderGui()
{
	float* lineWidthRange =  m_rDevice.properties.limits.lineWidthRange;	// lineWidthRange[2] is the range [minimum,maximum] of supported widths for lines
	float granularity = m_rDevice.properties.limits.lineWidthGranularity;	// granularity of supported point sizes

	if (ImGui::CollapsingHeader("Debug Pipeline"))	
		ImGui::DragFloat("LineWidth", &m_lineWidth, granularity, lineWidthRange[0], lineWidthRange[1]);
}