#pragma once

#include "glfw/glfw3.h"
#include <string>
#include <vector>
#include "render/core/fanShader.hpp"

namespace fan
{
	struct Device;

	//================================================================================================================================
	// base class for render pipelines
	//================================================================================================================================
	class Pipeline
	{
	public:
		Pipeline( Device& _device );
		virtual ~Pipeline();

		void Init( VkRenderPass _renderPass, const VkExtent2D _extent, const std::string _vertShaderPath, const std::string _fragShaderPath );
		void Create();

		virtual void Resize( const VkExtent2D _extent );
		virtual void ReloadShaders();
		virtual void Bind( VkCommandBuffer _commandBuffer, const size_t _index );
		virtual void UpdateUniformBuffers( const size_t _index ) = 0;

		VkPipeline	GetPipeline() { return m_pipeline; }
		VkPipelineLayout	GetLayout() { return m_pipelineLayout; }

	protected:
		Device& m_device;
		VkPipelineLayout	m_pipelineLayout;

		VkPipelineRasterizationStateCreateInfo	m_rasterizationStateCreateInfo = {};
		VkPipelineInputAssemblyStateCreateInfo	m_inputAssemblyStateCreateInfo = {};
		VkPipelineDepthStencilStateCreateInfo	m_depthStencilStateCreateInfo = {};
		VkPipelineMultisampleStateCreateInfo	m_multisampleStateCreateInfo = {};
		VkPipelineShaderStageCreateInfo			m_vertshaderStageCreateInfos = {};
		VkPipelineShaderStageCreateInfo			m_fragShaderStageCreateInfos = {};
		VkPipelineColorBlendStateCreateInfo		m_colorBlendStateCreateInfo = {};
		std::vector<VkVertexInputBindingDescription >	 m_bindingDescription;
		std::vector<VkVertexInputAttributeDescription >  m_attributeDescriptions;
		std::vector<VkPipelineColorBlendAttachmentState> m_attachmentBlendStates;
		std::vector<VkDescriptorSetLayout>				 m_descriptorSetLayouts;
		std::vector<VkPushConstantRange>				 m_pushConstantRanges;
		std::vector<VkDynamicState>						 m_dynamicStates;
		std::vector< VkViewport >						 m_viewports;
		std::vector<VkRect2D>							 m_scissors;

		virtual void ConfigurePipeline() = 0;
		bool CreatePipeline();
		void DeletePipeline();

	private:
		VkPipeline	 m_pipeline;
		VkRenderPass m_renderPass;

		VkExtent2D	m_extent;
		std::string m_fragShaderPath;
		std::string m_vertShaderPath;

		Shader m_fragmentShader;
		Shader m_vertexShader;

		void CreateShaders();
		void PreConfigurePipeline();
	};
}