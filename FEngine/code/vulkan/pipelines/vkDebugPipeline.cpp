#include "fanIncludes.h"

#include "vulkan/pipelines/vkDebugPipeline.h"
#include "vulkan/core/vkDevice.h"
#include "vulkan/core/vkShader.h"
#include "vulkan/core/vkImage.h"
#include "vulkan/core/vkImageView.h" 
#include "vulkan/core/vkBuffer.h"
#include "vulkan/vkRenderer.h"
#include "vulkan/util/vkVertex.h"

namespace vk {
	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::DebugPipeline(Device& _device, VkRenderPass& _renderPass, const VkPrimitiveTopology _primitiveTopology) :
		m_device(_device)
		, m_renderPass(_renderPass)
		, m_primitiveTopology(_primitiveTopology){
	}

	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::~DebugPipeline() {
		DeletePipeline();
		DeleteDescriptors();

		delete m_fragmentShader;
		delete m_vertexShader;
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Create( VkExtent2D _extent) {
		CreateShaders();
		CreateDescriptors();
		CreatePipeline(_extent);
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Resize(VkExtent2D _extent) {
		DeleteDescriptors();
		DeletePipeline();
		CreateDescriptors();
		CreatePipeline(_extent);

		SetUniforms(m_uniforms);
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::ReloadShaders() {
		m_vertexShader->Reload();
		m_fragmentShader->Reload();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::SetUniforms(const Uniforms _uniforms) {
		m_uniforms = _uniforms;
		m_uniformBuffer->SetData(&m_uniforms, sizeof(m_uniforms));
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Draw(VkCommandBuffer _commandBuffer, vk::Buffer& _vertexBuffer, const uint32_t _count) {
		if (_count > 0) {
			vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

			VkBuffer vertexBuffers[] = { _vertexBuffer.GetBuffer() };

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
			vkCmdDraw(_commandBuffer, static_cast<uint32_t>(_count), 1, 0, 0);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::CreateShaders() {
		delete m_fragmentShader;
		delete m_vertexShader;

		m_fragmentShader = new Shader(m_device);
		m_fragmentShader->Create("shaders/debug.frag");

		m_vertexShader = new Shader(m_device);
		m_vertexShader->Create("shaders/debug.vert");
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DebugPipeline::CreateDescriptors() {
		VkDescriptorSetLayoutBinding uboLayoutBinding;
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		std::vector< VkDescriptorSetLayoutBinding > layoutBindings = {
			uboLayoutBinding
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor set layout." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDescriptorSetLayout\t" << m_descriptorSetLayout << std::dec << std::endl;

		std::vector< VkDescriptorPoolSize > poolSizes(1);
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

		if (vkCreateDescriptorPool(m_device.vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDescriptorPool\t" << m_descriptorPool << std::dec << std::endl;

		std::vector< VkDescriptorSetLayout > descriptorSetLayouts = {
			m_descriptorSetLayout
		};

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		std::vector<VkDescriptorSet> descriptorSets(descriptorSetLayouts.size());
		if (vkAllocateDescriptorSets(m_device.vkDevice, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor set." << std::endl;
			return false;
		}
		m_descriptorSet = descriptorSets[0];
		std::cout << std::hex << "VkDescriptorSet\t\t" << m_descriptorSet << std::dec << std::endl;

		m_uniformBuffer = new Buffer(m_device);
		m_uniformBuffer->Create(
			sizeof(m_uniforms),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		VkDescriptorBufferInfo uboDescriptorBufferInfo = {};
		uboDescriptorBufferInfo.buffer = m_uniformBuffer->GetBuffer();
		uboDescriptorBufferInfo.offset = 0;
		uboDescriptorBufferInfo.range = sizeof(m_uniforms);

		VkWriteDescriptorSet uboWriteDescriptorSet = {};
		uboWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uboWriteDescriptorSet.pNext = nullptr;
		uboWriteDescriptorSet.dstSet = m_descriptorSet;
		uboWriteDescriptorSet.dstBinding = 0;
		uboWriteDescriptorSet.dstArrayElement = 0;
		uboWriteDescriptorSet.descriptorCount = 1;
		uboWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboWriteDescriptorSet.pImageInfo = nullptr;
		uboWriteDescriptorSet.pBufferInfo = &uboDescriptorBufferInfo;
		//uboWriteDescriptorSet.pTexelBufferView = nullptr;

		std::vector<VkWriteDescriptorSet> writeDescriptors = { uboWriteDescriptorSet };

		vkUpdateDescriptorSets(
			m_device.vkDevice,
			static_cast<uint32_t>(writeDescriptors.size()),
			writeDescriptors.data(),
			0,
			nullptr
		);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DebugPipeline::CreatePipeline( VkExtent2D _extent ) {
		VkPipelineShaderStageCreateInfo vertshaderStageCreateInfos = {};
		vertshaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertshaderStageCreateInfos.pNext = nullptr;
		vertshaderStageCreateInfos.flags = 0;
		vertshaderStageCreateInfos.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertshaderStageCreateInfos.module = m_vertexShader->GetModule();
		vertshaderStageCreateInfos.pName = "main";
		vertshaderStageCreateInfos.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfos = {};
		fragShaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfos.pNext = nullptr;
		fragShaderStageCreateInfos.flags = 0;
		fragShaderStageCreateInfos.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfos.module = m_fragmentShader->GetModule();
		fragShaderStageCreateInfos.pName = "main";
		fragShaderStageCreateInfos.pSpecializationInfo = nullptr;

		std::vector < VkPipelineShaderStageCreateInfo> shaderStages = { vertshaderStageCreateInfos, fragShaderStageCreateInfos };
		std::vector < VkVertexInputBindingDescription > bindingDescription = DebugVertex::GetBindingDescription();
		std::vector < VkVertexInputAttributeDescription > attributeDescriptions = DebugVertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescription.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = m_primitiveTopology;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		std::vector< VkViewport > viewports(1);
		viewports[0].x = 0.f;
		viewports[0].y = 0.f;
		viewports[0].width = static_cast<float> (_extent.width);
		viewports[0].height = static_cast<float> (_extent.height);
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
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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
		attachmentBlendStates[0].blendEnable = VK_TRUE;
		attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
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
			m_descriptorSetLayout
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

		if (vkCreatePipelineLayout(m_device.vkDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
			std::cout << "Could not allocate command pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkPipelineLayout\t" << m_pipelineLayout << std::dec << std::endl;

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
		graphicsPipelineCreateInfo.layout = m_pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = m_renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		std::vector<VkGraphicsPipelineCreateInfo> graphicsPipelineCreateInfos = {
			graphicsPipelineCreateInfo
		};

		std::vector<VkPipeline> graphicsPipelines(graphicsPipelineCreateInfos.size());

		if (vkCreateGraphicsPipelines(
			m_device.vkDevice,
			VK_NULL_HANDLE,
			static_cast<uint32_t>(graphicsPipelineCreateInfos.size()),
			graphicsPipelineCreateInfos.data(),
			nullptr,
			graphicsPipelines.data()
		) != VK_SUCCESS) {
			std::cout << "Could not allocate graphicsPipelines." << std::endl;
			return false;
		}

		m_pipeline = graphicsPipelines[0];

		for (int pipelineIndex = 0; pipelineIndex < graphicsPipelines.size(); pipelineIndex++) {
			std::cout << std::hex << "VkPipeline\t\t" << graphicsPipelines[pipelineIndex] << std::dec << std::endl;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::DeletePipeline() {
		if (m_pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(m_device.vkDevice, m_pipelineLayout, nullptr);
			m_pipelineLayout = VK_NULL_HANDLE;
		}

		if (m_pipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(m_device.vkDevice, m_pipeline, nullptr);
			m_pipeline = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::DeleteDescriptors() {

		if (m_descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_device.vkDevice, m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if (m_descriptorSetLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(m_device.vkDevice, m_descriptorSetLayout, nullptr);
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}
		delete m_uniformBuffer;
	}
}