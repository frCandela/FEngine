#include "Includes.h"

#include "vulkan/vkPostprocessPipeline.h"
#include "vulkan/vkDevice.h"
#include "vulkan/vkShader.h"
#include "vulkan/vkSampler.h"
#include "vulkan/vkImage.h"
#include "vulkan/vkImageView.h" 
#include "vulkan/vkBuffer.h"
#include "vulkan/vkRenderer.h"

namespace vk {	
	
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::PostprocessPipeline(Device * _device) : 
	m_device( _device ) {

		m_uniformsPostprocess.color = glm::vec4(1, 1, 1, 1);

		//Sampler
		m_samplerPostprocess = new Sampler(m_device);
		m_samplerPostprocess->CreateSampler(0, 1.f);

		CreateShaders();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::~PostprocessPipeline() {
		DeletePipelinePostprocess();
		//DeleteFramebuffersPostprocess();
		DeleteRenderPassPostprocess();
		DeleteDescriptorsPostprocess();
		DeletePostprocessRessources();
		delete m_vertexBufferPostprocess;
		delete m_samplerPostprocess;
		delete m_fragmentShaderPostprocess;
		delete m_vertexShaderPostprocess;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::Create( const VkFormat _format, VkExtent2D _extent) {
		CreateRenderPassPostprocess( _format );
		CreatePostprocessVertexBuffer();
		CreatePostprocessImages(_format, _extent);

		//CreateFramebuffersPostprocess();
		CreateDescriptorsPostprocess();
		CreatePipelinePostprocess( _extent );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::SetUniforms(const UniformsPostprocess _uniforms) {
		m_uniformsPostprocess = _uniforms;
		m_uniformBufferPostprocess->SetData(&m_uniformsPostprocess, sizeof(m_uniformsPostprocess));
	}
	//================================================================================================================================
	//================================================================================================================================
	VkImageView PostprocessPipeline::GetImageView() { return m_imageViewPostprocess->GetImageView(); }

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateShaders() {
		m_fragmentShaderPostprocess = new Shader(m_device);
		m_fragmentShaderPostprocess->Create("shaders/postprocess.frag");

		m_vertexShaderPostprocess = new Shader(m_device);
		m_vertexShaderPostprocess->Create("shaders/postprocess.vert");
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::ReloadShaders() {
		delete m_fragmentShaderPostprocess;
		delete m_vertexShaderPostprocess;
		CreateShaders();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::CreateDescriptorsPostprocess() {
		VkDescriptorSetLayoutBinding imageSamplerLayoutBinding;
		imageSamplerLayoutBinding.binding = 0;
		imageSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		imageSamplerLayoutBinding.descriptorCount = 1;
		imageSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		imageSamplerLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding uniformsLayoutBinding;
		uniformsLayoutBinding.binding = 1;
		uniformsLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformsLayoutBinding.descriptorCount = 1;
		uniformsLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		uniformsLayoutBinding.pImmutableSamplers = nullptr;

		std::vector< VkDescriptorSetLayoutBinding > layoutBindings = {
			imageSamplerLayoutBinding
			, uniformsLayoutBinding
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_device->vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayoutPostprocess) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor set layout." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDescriptorSetLayout\t" << m_descriptorSetLayoutPostprocess << std::dec << std::endl;

		std::vector< VkDescriptorPoolSize > poolSizes(2);
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[0].descriptorCount = 1;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

		if (vkCreateDescriptorPool(m_device->vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPoolPostprocess) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDescriptorPoolpp\t" << m_descriptorPoolPostprocess << std::dec << std::endl;

		std::vector< VkDescriptorSetLayout > descriptorSetLayouts = {
			m_descriptorSetLayoutPostprocess
		};

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPoolPostprocess;
		descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		std::vector<VkDescriptorSet> descriptorSets(descriptorSetLayouts.size());
		if (vkAllocateDescriptorSets(m_device->vkDevice, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor set." << std::endl;
			return false;
		}
		m_descriptorSetPostprocess = descriptorSets[0];
		std::cout << std::hex << "VkDescriptorSet\t\t" << m_descriptorSetPostprocess << std::dec << std::endl;

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImageInfo.imageView = m_imageViewPostprocess->GetImageView();
		descriptorImageInfo.sampler = m_samplerPostprocess->GetSampler();

		VkWriteDescriptorSet imageSamplerWriteDescriptorSet = {};
		imageSamplerWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		imageSamplerWriteDescriptorSet.pNext = nullptr;
		imageSamplerWriteDescriptorSet.dstSet = m_descriptorSetPostprocess;
		imageSamplerWriteDescriptorSet.dstBinding = 0;
		imageSamplerWriteDescriptorSet.dstArrayElement = 0;
		imageSamplerWriteDescriptorSet.descriptorCount = 1;
		imageSamplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		imageSamplerWriteDescriptorSet.pImageInfo = &descriptorImageInfo;
		imageSamplerWriteDescriptorSet.pBufferInfo = nullptr;
		//uboWriteDescriptorSet.pTexelBufferView = nullptr;

		m_uniformBufferPostprocess = new Buffer(m_device);
		m_uniformBufferPostprocess->Create(
			sizeof(UniformsPostprocess),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		VkDescriptorBufferInfo uniformsDescriptorBufferInfo = {};
		uniformsDescriptorBufferInfo.buffer = m_uniformBufferPostprocess->GetBuffer();
		uniformsDescriptorBufferInfo.offset = 0;
		uniformsDescriptorBufferInfo.range = sizeof(UniformsPostprocess);

		VkWriteDescriptorSet uniformsWriteDescriptorSet = {};
		uniformsWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uniformsWriteDescriptorSet.pNext = nullptr;
		uniformsWriteDescriptorSet.dstSet = m_descriptorSetPostprocess;
		uniformsWriteDescriptorSet.dstBinding = 1;
		uniformsWriteDescriptorSet.dstArrayElement = 0;
		uniformsWriteDescriptorSet.descriptorCount = 1;
		uniformsWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformsWriteDescriptorSet.pImageInfo = nullptr;
		uniformsWriteDescriptorSet.pBufferInfo = &uniformsDescriptorBufferInfo;

		std::vector<VkWriteDescriptorSet> writeDescriptors = { imageSamplerWriteDescriptorSet, uniformsWriteDescriptorSet };

		vkUpdateDescriptorSets(
			m_device->vkDevice,
			static_cast<uint32_t>(writeDescriptors.size()),
			writeDescriptors.data(),
			0,
			nullptr
		);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::CreateRenderPassPostprocess( const VkFormat _format ) {
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = _format;
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
			std::cout << "Could not create render pass pp;" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkRenderPass pp\t\t" << m_renderPassPostprocess << std::dec << std::endl;

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::CreatePipelinePostprocess( VkExtent2D _extent ) {
		VkPipelineShaderStageCreateInfo vertshaderStageCreateInfos = {};
		vertshaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertshaderStageCreateInfos.pNext = nullptr;
		vertshaderStageCreateInfos.flags = 0;
		vertshaderStageCreateInfos.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertshaderStageCreateInfos.module = m_vertexShaderPostprocess->GetModule();
		vertshaderStageCreateInfos.pName = "main";
		vertshaderStageCreateInfos.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfos = {};
		fragShaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfos.pNext = nullptr;
		fragShaderStageCreateInfos.flags = 0;
		fragShaderStageCreateInfos.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfos.module = m_fragmentShaderPostprocess->GetModule();
		fragShaderStageCreateInfos.pName = "main";
		fragShaderStageCreateInfos.pSpecializationInfo = nullptr;

		std::vector < VkPipelineShaderStageCreateInfo> shaderStages = { vertshaderStageCreateInfos, fragShaderStageCreateInfos };
		//std::vector < VkVertexInputBindingDescription > bindingDescription = Vertex::GetBindingDescription();
		//std::vector < VkVertexInputAttributeDescription > attributeDescriptions = Vertex::GetAttributeDescriptions();


		std::vector <VkVertexInputBindingDescription> bindingDescription(1);
		bindingDescription[0].binding = 0;								// Index of the binding in the array of bindings
		bindingDescription[0].stride = sizeof(glm::vec3);					// Number of bytes from one entry to the next
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = 0;

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescription.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		std::vector< VkViewport > viewports(1);
		viewports[0].x = 0.f;
		viewports[0].y = 0.f;
		viewports[0].width = static_cast<float> (_extent.width );
		viewports[0].height = static_cast<float> (_extent.height );
		viewports[0].minDepth = 0.0f;
		viewports[0].maxDepth = 1.0f;

		std::vector<VkRect2D> scissors(1);
		scissors[0].offset = { 0, 0 };
		scissors[0].extent = _extent;

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = nullptr;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = static_cast<uint32_t> (viewports.size());
		viewportStateCreateInfo.pViewports = viewports.data();
		viewportStateCreateInfo.scissorCount = static_cast<uint32_t> (scissors.size());;
		viewportStateCreateInfo.pScissors = scissors.data();

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = nullptr;
		rasterizationStateCreateInfo.flags = 0;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = nullptr;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		//multisampleStateCreateInfo.minSampleShading=;
		//multisampleStateCreateInfo.pSampleMask=;
		//multisampleStateCreateInfo.alphaToCoverageEnable=;
		//multisampleStateCreateInfo.alphaToOneEnable=;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.pNext = nullptr;
		depthStencilStateCreateInfo.flags = 0;
		depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		//depthStencilStateCreateInfo.front=;
		//depthStencilStateCreateInfo.back=;
		//depthStencilStateCreateInfo.minDepthBounds=;
		//depthStencilStateCreateInfo.maxDepthBounds=;

		std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates(1);
		attachmentBlendStates[0].blendEnable = VK_FALSE;
		attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		attachmentBlendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
		attachmentBlendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		attachmentBlendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		attachmentBlendStates[0].alphaBlendOp = VK_BLEND_OP_ADD;
		attachmentBlendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentBlendStates.size());
		colorBlendStateCreateInfo.pAttachments = attachmentBlendStates.data();
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
			m_descriptorSetLayoutPostprocess
		};
		std::vector<VkPushConstantRange> pushConstantRanges(0);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(m_device->vkDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayoutPostprocess) != VK_SUCCESS) {
			std::cout << "Could not allocate command pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkPipelineLayout\t" << m_pipelineLayoutPostprocess << std::dec << std::endl;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		graphicsPipelineCreateInfo.pStages = shaderStages.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = m_pipelineLayoutPostprocess;
		graphicsPipelineCreateInfo.renderPass = m_renderPassPostprocess;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		std::vector<VkGraphicsPipelineCreateInfo> graphicsPipelineCreateInfos = {
			graphicsPipelineCreateInfo
		};

		std::vector<VkPipeline> graphicsPipelines(graphicsPipelineCreateInfos.size());

		if (vkCreateGraphicsPipelines(
			m_device->vkDevice,
			VK_NULL_HANDLE,
			static_cast<uint32_t>(graphicsPipelineCreateInfos.size()),
			graphicsPipelineCreateInfos.data(),
			nullptr,
			graphicsPipelines.data()
		) != VK_SUCCESS) {
			std::cout << "Could not allocate graphicsPipelines." << std::endl;
			return false;
		}

		m_pipelinePostprocess = graphicsPipelines[0];

		for (int pipelineIndex = 0; pipelineIndex < graphicsPipelines.size(); pipelineIndex++) {
			std::cout << std::hex << "VkPipeline\t\t" << graphicsPipelines[pipelineIndex] << std::dec << std::endl;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreatePostprocessImages( const VkFormat _format, VkExtent2D _extent) {

		m_imagePostprocess = new Image(m_device);
		m_imagePostprocess->Create(
			_format,
			_extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_imageViewPostprocess = new ImageView(m_device);
		m_imageViewPostprocess->Create(m_imagePostprocess->GetImage(), _format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
	}

	void PostprocessPipeline::CreatePostprocessVertexBuffer() {
		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
		m_vertexBufferPostprocess = new Buffer(m_device);
		m_vertexBufferPostprocess->Create(
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		Buffer stagingBuffer(m_device);
		stagingBuffer.Create(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer.SetData(vertices.data(), size);
		VkCommandBuffer cmd = Renderer::GetGlobalRenderer()->BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo(cmd, m_vertexBufferPostprocess->GetBuffer(), size);
		Renderer::GetGlobalRenderer()->EndSingleTimeCommands(cmd);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::DeletePipelinePostprocess() {
		if (m_pipelineLayoutPostprocess != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(m_device->vkDevice, m_pipelineLayoutPostprocess, nullptr);
			m_pipelineLayoutPostprocess = VK_NULL_HANDLE;
		}

		if (m_pipelinePostprocess != VK_NULL_HANDLE) {
			vkDestroyPipeline(m_device->vkDevice, m_pipelinePostprocess, nullptr);
			m_pipelinePostprocess = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
//================================================================================================================================
	void PostprocessPipeline::DeleteRenderPassPostprocess() {
		if (m_renderPassPostprocess != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_device->vkDevice, m_renderPassPostprocess, nullptr);
			m_renderPassPostprocess = VK_NULL_HANDLE;
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::DeletePostprocessRessources() {
		delete m_imageViewPostprocess;
		delete m_imagePostprocess;

	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::DeleteDescriptorsPostprocess() {

		if (m_descriptorPoolPostprocess != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_device->vkDevice, m_descriptorPoolPostprocess, nullptr);
			m_descriptorPoolPostprocess = VK_NULL_HANDLE;
		}

		if (m_descriptorSetLayoutPostprocess != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(m_device->vkDevice, m_descriptorSetLayoutPostprocess, nullptr);
			m_descriptorSetLayoutPostprocess = VK_NULL_HANDLE;
		}
		delete m_uniformBufferPostprocess;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::RecordCommandBufferPostProcess( VkCommandBuffer _commandBuffer, VkFramebuffer _framebuffer ) {

		//VkCommandBuffer commandBuffer = m_postprocessCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = _framebuffer;
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if (vkBeginCommandBuffer(_commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelinePostprocess);

			VkBuffer vertexBuffers[] = { m_vertexBufferPostprocess->GetBuffer() };

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayoutPostprocess, 0, 1, &m_descriptorSetPostprocess, 0, nullptr);
			vkCmdDraw(_commandBuffer, static_cast<uint32_t>(4), 1, 0, 0);

			if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS) {
				std::cout << "Could not record command buffer " << _commandBuffer << "." << std::endl;
			}
		}
		else {
			std::cout << "Could not record command buffer " << _commandBuffer << "." << std::endl;
		}

	}
}