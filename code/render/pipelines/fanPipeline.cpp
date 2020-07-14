#include "render/pipelines/fanPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	PipelineConfig::PipelineConfig( Shader& _frag, Shader& _vert )
	{
		vertshaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertshaderStageCreateInfos.pNext = nullptr;
		vertshaderStageCreateInfos.flags = 0;
		vertshaderStageCreateInfos.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertshaderStageCreateInfos.module = _vert.mShaderModule;
		vertshaderStageCreateInfos.pName = "main";
		vertshaderStageCreateInfos.pSpecializationInfo = nullptr;

		fragShaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfos.pNext = nullptr;
		fragShaderStageCreateInfos.flags = 0;
		fragShaderStageCreateInfos.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfos.module = _frag.mShaderModule;
		fragShaderStageCreateInfos.pName = "main";
		fragShaderStageCreateInfos.pSpecializationInfo = nullptr;

		bindingDescription.clear();
		attributeDescriptions.clear();

		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

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

		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = nullptr;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		//multisampleStateCreateInfo.minSampleShading=;
		//multisampleStateCreateInfo.pSampleMask=;
		//multisampleStateCreateInfo.alphaToCoverageEnable=;
		//multisampleStateCreateInfo.alphaToOneEnable=;

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

		attachmentBlendStates.resize( 1 );
		attachmentBlendStates[0].blendEnable = VK_TRUE;
		attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		attachmentBlendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
		attachmentBlendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		attachmentBlendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		attachmentBlendStates[0].alphaBlendOp = VK_BLEND_OP_ADD;
		attachmentBlendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>( attachmentBlendStates.size() );
		colorBlendStateCreateInfo.pAttachments = attachmentBlendStates.data();
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		descriptorSetLayouts.clear();
		pushConstantRanges.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Destroy( Device& _device )
	{
		m_fragmentShader.Destroy( _device );
		m_vertexShader.Destroy( _device );
		DeletePipeline( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Init( VkRenderPass _renderPass, const VkExtent2D /*_extent*/, const std::string _vertShaderPath, const std::string _fragShaderPath )
	{
		m_renderPass = _renderPass;
		m_fragShaderPath = _fragShaderPath;
		m_vertShaderPath = _vertShaderPath;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Create( Device& _device, VkExtent2D _extent )
	{
		CreateShaders( _device );
		CreatePipeline( _device, GetConfig(), _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Pipeline::CreatePipeline( Device& _device, PipelineConfig _pipelineConfig, VkExtent2D _extent )
	{
		VkViewport viewport;
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float> ( _extent.width );
		viewport.height = static_cast<float> ( _extent.height );
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = _extent;

		assert( _pipelineConfig.bindingDescription.size() > 0 );
		assert( _pipelineConfig.attributeDescriptions.size() > 0 );

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast< uint32_t >( _pipelineConfig.bindingDescription.size() );
		vertexInputStateCreateInfo.pVertexBindingDescriptions = _pipelineConfig.bindingDescription.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast< uint32_t >( _pipelineConfig.attributeDescriptions.size() );
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = _pipelineConfig.attributeDescriptions.data();

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = nullptr;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		_pipelineConfig.colorBlendStateCreateInfo.attachmentCount = static_cast< uint32_t >( _pipelineConfig.attachmentBlendStates.size() );
		_pipelineConfig.colorBlendStateCreateInfo.pAttachments = _pipelineConfig.attachmentBlendStates.data();

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast< uint32_t >( _pipelineConfig.dynamicStates.size() );
		dynamicStateCreateInfo.pDynamicStates = _pipelineConfig.dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast< uint32_t >( _pipelineConfig.descriptorSetLayouts.size() );
		pipelineLayoutCreateInfo.pSetLayouts = _pipelineConfig.descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast< uint32_t >( _pipelineConfig.pushConstantRanges.size() );
		pipelineLayoutCreateInfo.pPushConstantRanges = _pipelineConfig.pushConstantRanges.data();

		if ( vkCreatePipelineLayout( _device.mDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command pool." );
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkPipelineLayout      " << m_pipelineLayout << std::dec << Debug::Endl();


		_pipelineConfig.fragShaderStageCreateInfos.module = m_fragmentShader.mShaderModule;
		_pipelineConfig.vertshaderStageCreateInfos.module = m_vertexShader.mShaderModule;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { _pipelineConfig.vertshaderStageCreateInfos, _pipelineConfig.fragShaderStageCreateInfos };

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast< uint32_t >( shaderStages.size() );
		graphicsPipelineCreateInfo.pStages = shaderStages.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &_pipelineConfig.inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &_pipelineConfig.rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &_pipelineConfig.multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = &_pipelineConfig.depthStencilStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &_pipelineConfig.colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = m_pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = m_renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		if ( vkCreateGraphicsPipelines(
			_device.mDevice,
			VK_NULL_HANDLE,
			1,
			&graphicsPipelineCreateInfo,
			nullptr,
			&m_pipeline
			) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate graphicsPipelines." );
			return false;
		}

		Debug::Get() << Debug::Severity::log << std::hex << "VkPipeline            " << m_pipeline << std::dec << Debug::Endl();
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::ReloadShaders( Device& _device, VkExtent2D _extent )
	{
		DeletePipeline( _device );
		m_vertexShader.Destroy( _device );
		m_fragmentShader.Destroy( _device );
		m_vertexShader.Create( _device, m_vertShaderPath);
		m_fragmentShader.Create( _device, m_fragShaderPath );
		CreatePipeline( _device, GetConfig(), _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t /*_index*/ )
	{
		VkViewport viewport;
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float> ( _extent.width );
		viewport.height = static_cast<float> ( _extent.height );
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = _extent;

		vkCmdBindPipeline( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline );
		vkCmdSetScissor( _commandBuffer, 0, 1, &scissor );
		vkCmdSetViewport( _commandBuffer, 0, 1, &viewport );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::CreateShaders( Device& _device )
	{
		assert( m_fragShaderPath != "" );
		assert( m_vertShaderPath != "" );

		m_fragmentShader.Create( _device, m_fragShaderPath );
		m_vertexShader.Create( _device, m_vertShaderPath );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::DeletePipeline( Device& _device )
	{
		if ( m_pipelineLayout != VK_NULL_HANDLE )
		{
			vkDestroyPipelineLayout( _device.mDevice, m_pipelineLayout, nullptr );
			m_pipelineLayout = VK_NULL_HANDLE;
		}

		if ( m_pipeline != VK_NULL_HANDLE )
		{
			vkDestroyPipeline( _device.mDevice, m_pipeline, nullptr );
			m_pipeline = VK_NULL_HANDLE;
		}
	}
}