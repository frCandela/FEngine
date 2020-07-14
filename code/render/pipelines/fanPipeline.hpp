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
		PipelineConfig( Shader& _frag, Shader& _vert );

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
		virtual void Destroy( Device& _device );

		void Init( VkRenderPass _renderPass, const VkExtent2D _extent, const std::string _vertShaderPath, const std::string _fragShaderPath );
		void Create( Device& _device, VkExtent2D _extent );

		virtual void ReloadShaders( Device& _device, VkExtent2D _extent );
		virtual void Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index );
		virtual void SetUniformsData( Device& _device, const size_t _index ) = 0;

		virtual PipelineConfig GetConfig() = 0;
		bool CreatePipeline( Device& _device, PipelineConfig _pipelineConfig, VkExtent2D _extent );
		void DeletePipeline( Device& _device );
		void CreateShaders( Device& _device );
		void PreConfigurePipeline();

		VkPipelineLayout	m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline			m_pipeline = VK_NULL_HANDLE;
		VkRenderPass		m_renderPass = VK_NULL_HANDLE;

		std::string			m_fragShaderPath;
		std::string			m_vertShaderPath;
		Shader				m_fragmentShader;
		Shader				m_vertexShader;




	};
}