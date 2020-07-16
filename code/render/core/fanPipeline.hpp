#pragma once

#include "glfw/glfw3.h"
#include <string>
#include <vector>
#include "render/core/fanShader.hpp"

namespace fan
{
	struct Device;

	//================================================================================================================================
	//================================================================================================================================
	struct PipelineConfig
	{
		PipelineConfig( const Shader& _vert, const Shader& _frag );

		VkPipelineRasterizationStateCreateInfo			rasterizationStateCreateInfo = {};
		VkPipelineInputAssemblyStateCreateInfo			inputAssemblyStateCreateInfo = {};
		VkPipelineDepthStencilStateCreateInfo			depthStencilStateCreateInfo = {};
		VkPipelineMultisampleStateCreateInfo			multisampleStateCreateInfo = {};
		VkPipelineShaderStageCreateInfo					vertshaderStageCreateInfos = {};
		VkPipelineShaderStageCreateInfo					fragShaderStageCreateInfos = {};
		VkPipelineColorBlendStateCreateInfo				colorBlendStateCreateInfo = {};
		std::vector<VkVertexInputBindingDescription >	bindingDescription;
		std::vector<VkVertexInputAttributeDescription > attributeDescriptions;
		std::vector<VkPipelineColorBlendAttachmentState>attachmentBlendStates;
		std::vector<VkDescriptorSetLayout>				descriptorSetLayouts;
		std::vector<VkPushConstantRange>				pushConstantRanges = {};
		std::vector<VkDynamicState>						dynamicStates;
	};

	//================================================================================================================================
	// base class for render pipelines
	//================================================================================================================================
	struct Pipeline
	{
		void Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent );

		bool Create( Device& _device, PipelineConfig _pipelineConfig, VkExtent2D _extent, VkRenderPass _renderPass, const bool _createCache = false );
		void Destroy( Device& _device );

		VkPipelineLayout	mPipelineLayout = VK_NULL_HANDLE;
		VkPipeline			mPipeline = VK_NULL_HANDLE;
		VkPipelineCache		mPipelineCache = VK_NULL_HANDLE;;
	};
}