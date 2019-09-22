#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanSampler.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::PostprocessPipeline(Device& _device, VkRenderPass& _renderPass) :
		m_device(_device)
		, m_renderPass(_renderPass) {

		m_uniforms.color = glm::vec4(1, 1, 1, 1);

		//Sampler
		m_sampler = new Sampler(m_device);
		m_sampler->CreateSampler(0, 1.f);

		CreateShaders();
	}

	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::~PostprocessPipeline() {
		DeletePipeline();
		DeleteDescriptors();
		DeleteImages();
		delete m_vertexBuffer;
		delete m_sampler;
		delete m_fragmentShader;
		delete m_vertexShader;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::Create(const VkFormat _format, VkExtent2D _extent) {
		m_format = _format;

		CreateVertexBuffer();
		CreateImagesAndViews(_extent);
		CreateDescriptors();
		CreatePipeline(_extent);

		Uniforms uniforms;
		uniforms.color = glm::vec4(1, 1, 1, 1);
		SetUniforms(uniforms);

		return true;
	}

	void PostprocessPipeline::Resize(VkExtent2D _extent) {
		DeleteImages();
		DeleteDescriptors();
		DeletePipeline();

		CreateImagesAndViews(_extent);
		CreateDescriptors();
		CreatePipeline(_extent);
		SetUniforms(m_uniforms);
	}

	void PostprocessPipeline::Draw(VkCommandBuffer _commandBuffer) {
		vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

		VkBuffer vertexBuffers[] = { m_vertexBuffer->GetBuffer() };

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
		vkCmdDraw(_commandBuffer, static_cast<uint32_t>(4), 1, 0, 0);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::SetUniforms(const Uniforms _uniforms) {
		m_uniforms = _uniforms;
		m_uniformBuffer->SetData(&m_uniforms, sizeof(m_uniforms));
	}
	//================================================================================================================================
	//================================================================================================================================
	VkImageView PostprocessPipeline::GetImageView() { return m_imageView->GetImageView(); }

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateShaders() {
		m_fragmentShader = new Shader(m_device);
		m_fragmentShader->Create("code/shaders/postprocess.frag");

		m_vertexShader = new Shader(m_device);
		m_vertexShader->Create("code/shaders/postprocess.vert");
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::ReloadShaders() {
		m_fragmentShader->Reload();
		m_vertexShader->Reload();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::CreateDescriptors() {
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

		if (vkCreateDescriptorSetLayout(m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
			Debug::Error("Could not allocate descriptor set layout.");
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSetLayout " << m_descriptorSetLayout << std::dec << Debug::Endl();

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

		if (vkCreateDescriptorPool(m_device.vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
			Debug::Error("Could not allocate descriptor pool.");
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorPoolpp    " << m_descriptorPool << std::dec << Debug::Endl();

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
			Debug::Error("Could not allocate descriptor set.");
			return false;
		}
		m_descriptorSet = descriptorSets[0];
		Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSet       " << m_descriptorSet << std::dec << Debug::Endl();

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImageInfo.imageView = m_imageView->GetImageView();
		descriptorImageInfo.sampler = m_sampler->GetSampler();

		VkWriteDescriptorSet imageSamplerWriteDescriptorSet = {};
		imageSamplerWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		imageSamplerWriteDescriptorSet.pNext = nullptr;
		imageSamplerWriteDescriptorSet.dstSet = m_descriptorSet;
		imageSamplerWriteDescriptorSet.dstBinding = 0;
		imageSamplerWriteDescriptorSet.dstArrayElement = 0;
		imageSamplerWriteDescriptorSet.descriptorCount = 1;
		imageSamplerWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		imageSamplerWriteDescriptorSet.pImageInfo = &descriptorImageInfo;
		imageSamplerWriteDescriptorSet.pBufferInfo = nullptr;
		//uboWriteDescriptorSet.pTexelBufferView = nullptr;

		m_uniformBuffer = new Buffer(m_device);
		m_uniformBuffer->Create(
			sizeof(Uniforms),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		VkDescriptorBufferInfo uniformsDescriptorBufferInfo = {};
		uniformsDescriptorBufferInfo.buffer = m_uniformBuffer->GetBuffer();
		uniformsDescriptorBufferInfo.offset = 0;
		uniformsDescriptorBufferInfo.range = sizeof(Uniforms);

		VkWriteDescriptorSet uniformsWriteDescriptorSet = {};
		uniformsWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uniformsWriteDescriptorSet.pNext = nullptr;
		uniformsWriteDescriptorSet.dstSet = m_descriptorSet;
		uniformsWriteDescriptorSet.dstBinding = 1;
		uniformsWriteDescriptorSet.dstArrayElement = 0;
		uniformsWriteDescriptorSet.descriptorCount = 1;
		uniformsWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformsWriteDescriptorSet.pImageInfo = nullptr;
		uniformsWriteDescriptorSet.pBufferInfo = &uniformsDescriptorBufferInfo;

		std::vector<VkWriteDescriptorSet> writeDescriptors = { imageSamplerWriteDescriptorSet, uniformsWriteDescriptorSet };

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
	bool PostprocessPipeline::CreatePipeline(VkExtent2D _extent) {
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
			Debug::Error("Could not allocate command pool.");
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkPipelineLayout      " << m_pipelineLayout << std::dec << Debug::Endl();

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
			Debug::Error("Could not allocate graphicsPipelines.");
			return false;
		}

		m_pipeline = graphicsPipelines[0];

		for (int pipelineIndex = 0; pipelineIndex < graphicsPipelines.size(); pipelineIndex++) {
			Debug::Get() << Debug::Severity::log << std::hex << "VkPipeline            " << graphicsPipelines[pipelineIndex] << std::dec << Debug::Endl();
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateImagesAndViews(VkExtent2D _extent) {

		m_image = new Image(m_device);
		m_image->Create(
			m_format,
			_extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_imageView = new ImageView(m_device);
		m_imageView->Create(m_image->GetImage(), m_format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
	}

	void PostprocessPipeline::CreateVertexBuffer() {
		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
		m_vertexBuffer = new Buffer(m_device);
		m_vertexBuffer->Create(
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
		VkCommandBuffer cmd = Renderer::Get().BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo(cmd, m_vertexBuffer->GetBuffer(), size);
		Renderer::Get().EndSingleTimeCommands(cmd);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::DeletePipeline() {
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
	void PostprocessPipeline::DeleteImages() {
		delete m_imageView;
		delete m_image;

	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::DeleteDescriptors() {

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