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
		PipelineConfig( Shader& _vert, Shader& _frag );

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
		std::vector<VkPushConstantRange>				pushConstantRanges;
		std::vector<VkDynamicState>						dynamicStates;
	};

	//================================================================================================================================
	// base class for render pipelines
	//================================================================================================================================
	struct Pipeline
	{
		void Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index );

		bool Create( Device& _device, PipelineConfig _pipelineConfig, VkExtent2D _extent, VkRenderPass _renderPass );
		void Destroy( Device& _device );

		VkPipelineLayout	mPipelineLayout = VK_NULL_HANDLE;
		VkPipeline			mPipeline = VK_NULL_HANDLE;
	};
}