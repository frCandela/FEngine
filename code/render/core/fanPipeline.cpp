#include "render/core/fanPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    PipelineConfig::PipelineConfig( const Shader& _vert, const Shader& _frag )
    {
        vertshaderStageInfos.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertshaderStageInfos.pNext               = nullptr;
        vertshaderStageInfos.flags               = 0;
        vertshaderStageInfos.stage               = VK_SHADER_STAGE_VERTEX_BIT;
        vertshaderStageInfos.module              = _vert.mShaderModule;
        vertshaderStageInfos.pName               = "main";
        vertshaderStageInfos.pSpecializationInfo = nullptr;

        fragShaderStageInfos.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfos.pNext               = nullptr;
        fragShaderStageInfos.flags               = 0;
        fragShaderStageInfos.stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfos.module              = _frag.mShaderModule;
        fragShaderStageInfos.pName               = "main";
        fragShaderStageInfos.pSpecializationInfo = nullptr;

        bindingDescription.clear();
        attributeDescriptions.clear();

        inputAssemblyStateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.pNext                  = nullptr;
        inputAssemblyStateInfo.flags                  = 0;
        inputAssemblyStateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

        rasterizationStateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateInfo.pNext                   = nullptr;
        rasterizationStateInfo.flags                   = 0;
        rasterizationStateInfo.depthClampEnable        = VK_FALSE;
        rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizationStateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizationStateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateInfo.depthBiasEnable         = VK_FALSE;
        rasterizationStateInfo.depthBiasConstantFactor = 0.0f;
        rasterizationStateInfo.depthBiasClamp          = 0.0f;
        rasterizationStateInfo.depthBiasSlopeFactor    = 0.0f;
        rasterizationStateInfo.lineWidth               = 1.0f;

        multisampleStateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateInfo.pNext                = nullptr;
        multisampleStateInfo.flags                = 0;
        multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateInfo.sampleShadingEnable  = VK_FALSE;
        //multisampleStateCreateInfo.minSampleShading=;
        //multisampleStateCreateInfo.pSampleMask=;
        //multisampleStateCreateInfo.alphaToCoverageEnable=;
        //multisampleStateCreateInfo.alphaToOneEnable=;

        depthStencilStateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateInfo.pNext                 = nullptr;
        depthStencilStateInfo.flags                 = 0;
        depthStencilStateInfo.depthTestEnable       = VK_TRUE;
        depthStencilStateInfo.depthWriteEnable      = VK_TRUE;
        depthStencilStateInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
        depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateInfo.stencilTestEnable     = VK_FALSE;
        //depthStencilStateCreateInfo.front=;
        //depthStencilStateCreateInfo.back=;
        //depthStencilStateCreateInfo.minDepthBounds=;
        //depthStencilStateCreateInfo.maxDepthBounds=;

        attachmentBlendStates.resize( 1 );
        attachmentBlendStates[0].blendEnable         = VK_TRUE;
        attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        attachmentBlendStates[0].colorBlendOp        = VK_BLEND_OP_ADD;
        attachmentBlendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachmentBlendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachmentBlendStates[0].alphaBlendOp        = VK_BLEND_OP_ADD;
        attachmentBlendStates[0].colorWriteMask      = VK_COLOR_COMPONENT_R_BIT |
                                                       VK_COLOR_COMPONENT_G_BIT |
                                                       VK_COLOR_COMPONENT_B_BIT |
                                                       VK_COLOR_COMPONENT_A_BIT;

        colorBlendStateInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateInfo.pNext           = nullptr;
        colorBlendStateInfo.flags           = 0;
        colorBlendStateInfo.logicOpEnable   = VK_FALSE;
        colorBlendStateInfo.logicOp         = VK_LOGIC_OP_COPY;
        colorBlendStateInfo.attachmentCount = static_cast<uint32_t>( attachmentBlendStates.size() );
        colorBlendStateInfo.pAttachments    = attachmentBlendStates.data();
        colorBlendStateInfo.blendConstants[0] = 0.0f;
        colorBlendStateInfo.blendConstants[1] = 0.0f;
        colorBlendStateInfo.blendConstants[2] = 0.0f;
        colorBlendStateInfo.blendConstants[3] = 0.0f;

        dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        descriptorSetLayouts.clear();
        pushConstantRanges.clear();
    }

    //========================================================================================================
    //========================================================================================================
    bool Pipeline::Create( Device& _device,
                           PipelineConfig _pipelineConfig,
                           VkExtent2D _extent,
                           VkRenderPass _renderPass,
                           const bool _createCache )
    {
        assert( mPipelineLayout == VK_NULL_HANDLE );
        assert( mPipeline == VK_NULL_HANDLE );

        if( _createCache )
        {
            assert( mPipelineCache == VK_NULL_HANDLE );
            VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            vkCreatePipelineCache( _device.mDevice, &pipelineCacheCreateInfo, nullptr, &mPipelineCache );
        }

        VkViewport viewport;
        viewport.x        = 0.f;
        viewport.y        = 0.f;
        viewport.width    = static_cast<float> ( _extent.width );
        viewport.height   = static_cast<float> ( _extent.height );
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = _extent;

        assert( _pipelineConfig.bindingDescription.size() > 0 );
        assert( _pipelineConfig.attributeDescriptions.size() > 0 );

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
        vertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.pNext                           = nullptr;
        vertexInputStateCreateInfo.flags                           = 0;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount   = static_cast< uint32_t >( _pipelineConfig.bindingDescription.size() );
        vertexInputStateCreateInfo.pVertexBindingDescriptions      = _pipelineConfig.bindingDescription.data();
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast< uint32_t >( _pipelineConfig.attributeDescriptions.size() );
        vertexInputStateCreateInfo.pVertexAttributeDescriptions    = _pipelineConfig.attributeDescriptions.data();

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.pNext         = nullptr;
        viewportStateCreateInfo.flags         = 0;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports    = &viewport;
        viewportStateCreateInfo.scissorCount  = 1;
        viewportStateCreateInfo.pScissors     = &scissor;

        _pipelineConfig.colorBlendStateInfo.attachmentCount = static_cast< uint32_t >( _pipelineConfig.attachmentBlendStates.size() );
        _pipelineConfig.colorBlendStateInfo.pAttachments    = _pipelineConfig.attachmentBlendStates.data();

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
        dynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.pNext             = nullptr;
        dynamicStateCreateInfo.flags             = 0;
        dynamicStateCreateInfo.dynamicStateCount = static_cast< uint32_t >( _pipelineConfig.dynamicStates.size() );
        dynamicStateCreateInfo.pDynamicStates    = _pipelineConfig.dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext                  = nullptr;
        pipelineLayoutCreateInfo.flags                  = 0;
        pipelineLayoutCreateInfo.setLayoutCount         = static_cast< uint32_t >( _pipelineConfig.descriptorSetLayouts.size() );
        pipelineLayoutCreateInfo.pSetLayouts            = _pipelineConfig.descriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast< uint32_t >( _pipelineConfig.pushConstantRanges.size() );
        pipelineLayoutCreateInfo.pPushConstantRanges    = _pipelineConfig.pushConstantRanges.data();

        if( vkCreatePipelineLayout( _device.mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout ) !=
            VK_SUCCESS )
        {
            Debug::Error( "Could not allocate command pool." );
            return false;
        }
        Debug::Get()
                << Debug::Severity::log
                << std::hex
                << "VkPipelineLayout      "
                << mPipelineLayout
                << std::dec
                << Debug::Endl();

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { _pipelineConfig.vertshaderStageInfos,
                                                                      _pipelineConfig.fragShaderStageInfos };

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.pNext               = nullptr;
        graphicsPipelineCreateInfo.flags               = 0;
        graphicsPipelineCreateInfo.stageCount          = static_cast< uint32_t >( shaderStages.size() );
        graphicsPipelineCreateInfo.pStages             = shaderStages.data();
        graphicsPipelineCreateInfo.pVertexInputState   = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &_pipelineConfig.inputAssemblyStateInfo;
        graphicsPipelineCreateInfo.pTessellationState  = nullptr;
        graphicsPipelineCreateInfo.pViewportState      = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &_pipelineConfig.rasterizationStateInfo;
        graphicsPipelineCreateInfo.pMultisampleState   = &_pipelineConfig.multisampleStateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState       = &_pipelineConfig.depthStencilStateInfo;
        graphicsPipelineCreateInfo.pColorBlendState         = &_pipelineConfig.colorBlendStateInfo;
        graphicsPipelineCreateInfo.pDynamicState            = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout              = mPipelineLayout;
        graphicsPipelineCreateInfo.renderPass          = _renderPass;
        graphicsPipelineCreateInfo.subpass             = 0;
        graphicsPipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex   = -1;

        if( vkCreateGraphicsPipelines(
                _device.mDevice,
                mPipelineCache,
                1,
                &graphicsPipelineCreateInfo,
                nullptr,
                &mPipeline
        ) != VK_SUCCESS )
        {
            Debug::Error( "Could not allocate graphicsPipelines." );
            return false;
        }

        _device.AddDebugName( (uint64_t)mPipelineLayout, "Pipeline" );
        _device.AddDebugName( (uint64_t)mPipeline, "Pipeline" );

        Debug::Get()
                << Debug::Severity::log
                << std::hex
                << "VkPipeline            "
                << mPipeline
                << std::dec
                << Debug::Endl();
        return true;
    }

    //================================================================================================================================
    //================================================================================================================================
    void Pipeline::Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent )
    {
        VkViewport viewport;
        viewport.x        = 0.f;
        viewport.y        = 0.f;
        viewport.width    = static_cast<float> ( _extent.width );
        viewport.height   = static_cast<float> ( _extent.height );
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = _extent;

        vkCmdSetScissor( _commandBuffer, 0, 1, &scissor );
        vkCmdSetViewport( _commandBuffer, 0, 1, &viewport );

        vkCmdBindPipeline( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline );
    }

    //================================================================================================================================
    //================================================================================================================================
    void Pipeline::Destroy( Device& _device )
    {
        if( mPipelineCache != VK_NULL_HANDLE )
        {
            vkDestroyPipelineCache( _device.mDevice, mPipelineCache, nullptr );
            _device.RemoveDebugName( (uint64_t)mPipelineCache );
            mPipelineCache = VK_NULL_HANDLE;
        }

        if( mPipelineLayout != VK_NULL_HANDLE )
        {
            vkDestroyPipelineLayout( _device.mDevice, mPipelineLayout, nullptr );
            _device.RemoveDebugName( (uint64_t)mPipelineLayout );
            mPipelineLayout = VK_NULL_HANDLE;
        }

        if( mPipeline != VK_NULL_HANDLE )
        {
            vkDestroyPipeline( _device.mDevice, mPipeline, nullptr );
            _device.RemoveDebugName( (uint64_t)mPipeline );
            mPipeline = VK_NULL_HANDLE;
        }
    }
}