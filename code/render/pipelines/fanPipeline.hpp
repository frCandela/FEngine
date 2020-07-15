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
		virtual void Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index );
		virtual void SetUniformsData( Device& /*_device*/, const size_t /*_index*/ ) {};

		bool CreatePipeline( Device& _device, PipelineConfig _pipelineConfig, VkExtent2D _extent, VkRenderPass _renderPass );
		void DeletePipeline( Device& _device );
		void PreConfigurePipeline();

		VkPipelineLayout	m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline			m_pipeline = VK_NULL_HANDLE;
	};
}