#include "render/core/fanPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Pipeline::Pipeline( Device& _device ) :
		m_device( _device )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	Pipeline::~Pipeline()
	{
		m_fragmentShader.Destroy( m_device );
		m_vertexShader.Destroy( m_device );
		DeletePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Init( VkRenderPass _renderPass, const VkExtent2D _extent, const std::string _vertShaderPath, const std::string _fragShaderPath )
	{
		m_renderPass = _renderPass;
		m_extent = _extent;
		m_fragShaderPath = _fragShaderPath;
		m_vertShaderPath = _vertShaderPath;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Create()
	{
		CreateShaders();
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Pipeline::CreatePipeline()
	{
		PreConfigurePipeline();
		ConfigurePipeline();

		assert( m_bindingDescription.size() > 0 );
		assert( m_attributeDescriptions.size() > 0 );

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast< uint32_t >( m_bindingDescription.size() );
		vertexInputStateCreateInfo.pVertexBindingDescriptions = m_bindingDescription.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast< uint32_t >( m_attributeDescriptions.size() );
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = m_attributeDescriptions.data();

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = nullptr;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = static_cast< uint32_t > ( m_viewports.size() );
		viewportStateCreateInfo.pViewports = m_viewports.data();
		viewportStateCreateInfo.scissorCount = static_cast< uint32_t > ( m_scissors.size() );;
		viewportStateCreateInfo.pScissors = m_scissors.data();

		m_colorBlendStateCreateInfo.attachmentCount = static_cast< uint32_t >( m_attachmentBlendStates.size() );
		m_colorBlendStateCreateInfo.pAttachments = m_attachmentBlendStates.data();

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast< uint32_t >( m_dynamicStates.size() );
		dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast< uint32_t >( m_descriptorSetLayouts.size() );
		pipelineLayoutCreateInfo.pSetLayouts = m_descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast< uint32_t >( m_pushConstantRanges.size() );
		pipelineLayoutCreateInfo.pPushConstantRanges = m_pushConstantRanges.data();

		if ( vkCreatePipelineLayout( m_device.mDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout ) != VK_SUCCESS )
		{
			Debug::Error( "Could not allocate command pool." );
			return false;
		}
		Debug::Get() << Debug::Severity::log << std::hex << "VkPipelineLayout      " << m_pipelineLayout << std::dec << Debug::Endl();


		m_fragShaderStageCreateInfos.module = m_fragmentShader.mShaderModule;
		m_vertshaderStageCreateInfos.module = m_vertexShader.mShaderModule;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { m_vertshaderStageCreateInfos, m_fragShaderStageCreateInfos };

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast< uint32_t >( shaderStages.size() );
		graphicsPipelineCreateInfo.pStages = shaderStages.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &m_inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &m_rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &m_multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = &m_depthStencilStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &m_colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = m_pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = m_renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		if ( vkCreateGraphicsPipelines(
			m_device.mDevice,
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
	void Pipeline::PreConfigurePipeline()
	{
		m_vertshaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_vertshaderStageCreateInfos.pNext = nullptr;
		m_vertshaderStageCreateInfos.flags = 0;
		m_vertshaderStageCreateInfos.stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_vertshaderStageCreateInfos.module = m_vertexShader.mShaderModule;
		m_vertshaderStageCreateInfos.pName = "main";
		m_vertshaderStageCreateInfos.pSpecializationInfo = nullptr;

		m_fragShaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_fragShaderStageCreateInfos.pNext = nullptr;
		m_fragShaderStageCreateInfos.flags = 0;
		m_fragShaderStageCreateInfos.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_fragShaderStageCreateInfos.module = m_fragmentShader.mShaderModule;
		m_fragShaderStageCreateInfos.pName = "main";
		m_fragShaderStageCreateInfos.pSpecializationInfo = nullptr;

		m_bindingDescription.clear();
		m_attributeDescriptions.clear();

		m_inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_inputAssemblyStateCreateInfo.pNext = nullptr;
		m_inputAssemblyStateCreateInfo.flags = 0;
		m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		m_viewports.resize( 1 );
		m_viewports[ 0 ].x = 0.f;
		m_viewports[ 0 ].y = 0.f;
		m_viewports[ 0 ].width = static_cast< float > ( m_extent.width );
		m_viewports[ 0 ].height = static_cast< float > ( m_extent.height );
		m_viewports[ 0 ].minDepth = 0.0f;
		m_viewports[ 0 ].maxDepth = 1.0f;

		m_scissors.resize( 1 );
		m_scissors[ 0 ].offset = { 0, 0 };
		m_scissors[ 0 ].extent = m_extent;

		m_rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_rasterizationStateCreateInfo.pNext = nullptr;
		m_rasterizationStateCreateInfo.flags = 0;
		m_rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		m_rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		m_rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		m_rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		m_rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		m_rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		m_rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		m_rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		m_rasterizationStateCreateInfo.lineWidth = 1.0f;

		m_multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_multisampleStateCreateInfo.pNext = nullptr;
		m_multisampleStateCreateInfo.flags = 0;
		m_multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		//multisampleStateCreateInfo.minSampleShading=;
		//multisampleStateCreateInfo.pSampleMask=;
		//multisampleStateCreateInfo.alphaToCoverageEnable=;
		//multisampleStateCreateInfo.alphaToOneEnable=;

		m_depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		m_depthStencilStateCreateInfo.pNext = nullptr;
		m_depthStencilStateCreateInfo.flags = 0;
		m_depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		m_depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		m_depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		m_depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		m_depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		//depthStencilStateCreateInfo.front=;
		//depthStencilStateCreateInfo.back=;
		//depthStencilStateCreateInfo.minDepthBounds=;
		//depthStencilStateCreateInfo.maxDepthBounds=;

		m_attachmentBlendStates.resize( 1 );
		m_attachmentBlendStates[ 0 ].blendEnable = VK_TRUE;
		m_attachmentBlendStates[ 0 ].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_attachmentBlendStates[ 0 ].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_attachmentBlendStates[ 0 ].colorBlendOp = VK_BLEND_OP_ADD;
		m_attachmentBlendStates[ 0 ].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_attachmentBlendStates[ 0 ].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_attachmentBlendStates[ 0 ].alphaBlendOp = VK_BLEND_OP_ADD;
		m_attachmentBlendStates[ 0 ].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		m_colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_colorBlendStateCreateInfo.pNext = nullptr;
		m_colorBlendStateCreateInfo.flags = 0;
		m_colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		m_colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		m_colorBlendStateCreateInfo.attachmentCount = static_cast< uint32_t >( m_attachmentBlendStates.size() );
		m_colorBlendStateCreateInfo.pAttachments = m_attachmentBlendStates.data();
		m_colorBlendStateCreateInfo.blendConstants[ 0 ] = 0.0f;
		m_colorBlendStateCreateInfo.blendConstants[ 1 ] = 0.0f;
		m_colorBlendStateCreateInfo.blendConstants[ 2 ] = 0.0f;
		m_colorBlendStateCreateInfo.blendConstants[ 3 ] = 0.0f;

		m_dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		m_descriptorSetLayouts.clear();
		m_pushConstantRanges.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Resize( const VkExtent2D _extent )
	{
		m_extent = _extent;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::ReloadShaders()
	{
		DeletePipeline();
		m_vertexShader.Destroy( m_device );
		m_fragmentShader.Destroy( m_device );
		m_vertexShader.Create( m_device, m_vertShaderPath);
		m_fragmentShader.Create( m_device, m_fragShaderPath );
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::Bind( VkCommandBuffer _commandBuffer, const size_t /*_index*/ )
	{
		m_viewports[ 0 ].x = 0.f;
		m_viewports[ 0 ].y = 0.f;
		m_viewports[ 0 ].width = static_cast< float > ( m_extent.width );
		m_viewports[ 0 ].height = static_cast< float > ( m_extent.height );
		m_viewports[ 0 ].minDepth = 0.0f;
		m_viewports[ 0 ].maxDepth = 1.0f;

		m_scissors[ 0 ].offset = { 0, 0 };
		m_scissors[ 0 ].extent = m_extent;

		vkCmdBindPipeline( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline );
		vkCmdSetScissor( _commandBuffer, 0, static_cast< uint32_t >( m_scissors.size() ), m_scissors.data() );
		vkCmdSetViewport( _commandBuffer, 0, static_cast< uint32_t >( m_viewports.size() ), m_viewports.data() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::CreateShaders()
	{
		assert( m_fragShaderPath != "" );
		assert( m_vertShaderPath != "" );

		m_fragmentShader.Create( m_device, m_fragShaderPath );
		m_vertexShader.Create( m_device, m_vertShaderPath );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Pipeline::DeletePipeline()
	{
		if ( m_pipelineLayout != VK_NULL_HANDLE )
		{
			vkDestroyPipelineLayout( m_device.mDevice, m_pipelineLayout, nullptr );
			m_pipelineLayout = VK_NULL_HANDLE;
		}

		if ( m_pipeline != VK_NULL_HANDLE )
		{
			vkDestroyPipeline( m_device.mDevice, m_pipeline, nullptr );
			m_pipeline = VK_NULL_HANDLE;
		}
	}
}