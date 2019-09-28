#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanSampler.h"
#include "renderer/core/fanDescriptor.h"
#include "renderer/fanRenderer.h"
#include "renderer/util/fanVertex.h"
#include "renderer/fanMesh.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::ForwardPipeline(Device& _device, VkRenderPass& _renderPass) :
		m_device(_device)
		, m_renderPass(_renderPass)
		, m_sceneDescriptor(nullptr){

		m_sampler = new Sampler(_device);
		m_sampler->CreateSampler(0, 8);
	}

	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::~ForwardPipeline() {
		DeletePipeline();
		DeleteDepthRessources();
		DeleteDescriptors();

		delete m_fragmentShader;
		delete m_vertexShader;
		delete m_sampler;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Create(VkExtent2D _extent) {
		CreateShaders();
		CreateDepthRessources(_extent);
		CreateDescriptors();
		CreatePipeline(_extent);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Resize(VkExtent2D _extent) {
		DeleteDepthRessources();
		DeleteDescriptors();
		DeletePipeline();

		CreateDepthRessources(_extent);
		CreateDescriptors();
		CreatePipeline(_extent);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ReloadShaders() {
		m_vertexShader->Reload();
		m_fragmentShader->Reload();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::SetUniformPointers( 
		LightsUniforms * _lightUniforms,
		AlignedMemory<DynamicUniformsVert>* _dynamicUniformsVert, 
		AlignedMemory<DynamicUniformsMaterial>* _dynamicUniformsMaterial,
		VertUniforms * _vertUniforms,
		FragUniforms * _fragUniforms )
	{
		m_vertUniforms = _vertUniforms;
		m_dynamicUniformsVert = _dynamicUniformsVert;
 		m_dynamicUniformsMaterial = _dynamicUniformsMaterial;
 		m_fragUniforms = _fragUniforms;
 		m_lightUniforms = _lightUniforms;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreateDescriptors() {
		delete m_sceneDescriptor;
		m_sceneDescriptor = new Descriptor( m_device );
		m_sceneDescriptor->AddUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		m_sceneDescriptor->AddDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert->GetTotalSize(), m_dynamicUniformsVert->GetAlignment() );
		m_sceneDescriptor->AddUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_sceneDescriptor->AddDynamicUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, m_dynamicUniformsMaterial->GetTotalSize(), m_dynamicUniformsMaterial->GetAlignment() );
		m_sceneDescriptor->AddUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );

		delete m_texturesDescriptor;
		m_texturesDescriptor = new  Descriptor( m_device );
		std::vector< Texture * > & textures = Renderer::Get().GetRessourceManager()->GetTextures();
		std::vector< VkImageView > imageViews( textures .size());
		for (int textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
			imageViews[textureIndex] = textures[textureIndex]->GetImageView();
		}
		m_texturesDescriptor->AddImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, imageViews, m_sampler->GetSampler() );

		return m_sceneDescriptor->Create() && m_texturesDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::UpdateUniformBuffers() {
		m_sceneDescriptor->SetBinding( 0, &( *m_vertUniforms ), sizeof( VertUniforms ) );
		m_sceneDescriptor->SetBinding( 1, &( *m_dynamicUniformsVert )[0], m_dynamicUniformsVert->GetTotalSize() );
		m_sceneDescriptor->SetBinding( 2,  &( *m_fragUniforms ), sizeof( FragUniforms ) );
		m_sceneDescriptor->SetBinding( 3, &( *m_dynamicUniformsMaterial )[0], m_dynamicUniformsMaterial->GetTotalSize() );
		m_sceneDescriptor->SetBinding( 4, &( *m_lightUniforms ), sizeof( LightsUniforms ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkImageView	ForwardPipeline::GetDepthImageView() {
		return m_depthImageView->GetImageView();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Draw(VkCommandBuffer _commandBuffer, const std::array< Mesh *, s_maximumNumModels > _meshArray, const uint32_t _num ) {
		vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

		VkDeviceSize offsets[] = { 0 };

		for (uint32_t meshIndex = 0; meshIndex < _num; meshIndex++) {
			Mesh * mesh = _meshArray[meshIndex];

			if (mesh != nullptr) {
				assert( mesh->GetVertexBuffer() != nullptr );
				assert( mesh->GetIndexBuffer() != nullptr );
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer()->GetBuffer() };
				vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(_commandBuffer, mesh->GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

				std::vector<uint32_t> dynamicOffsets = {
					meshIndex  * static_cast<uint32_t>( m_dynamicUniformsVert->GetAlignment())
					,meshIndex  * static_cast<uint32_t>( m_dynamicUniformsMaterial->GetAlignment() )
				};

				std::vector<VkDescriptorSet> descriptors = {
					m_sceneDescriptor->GetSet()
					, m_texturesDescriptor->GetSet()
				};

				vkCmdBindDescriptorSets(
					_commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_pipelineLayout,
					0,
					static_cast<uint32_t>(descriptors.size()),
					descriptors.data(),
					static_cast<uint32_t>(dynamicOffsets.size()),
					dynamicOffsets.data()
				);
				vkCmdDrawIndexed(_commandBuffer, static_cast<uint32_t>(mesh->GetIndices().size()), 1, 0, 0, 0);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::CreateShaders() {
		delete m_fragmentShader;
		delete m_vertexShader;

		m_fragmentShader = new Shader(m_device);
		m_fragmentShader->Create("code/shaders/forward.frag");

		m_vertexShader = new Shader(m_device);
		m_vertexShader->Create("code/shaders/forward.vert");
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreateDepthRessources(VkExtent2D _extent) {
		VkFormat depthFormat = m_device.FindDepthFormat();
		m_depthImage = new Image(m_device);
		m_depthImageView = new ImageView(m_device);
		m_depthImage->Create(depthFormat, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthImageView->Create(m_depthImage->GetImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);

		VkCommandBuffer cmd = Renderer::Get().BeginSingleTimeCommands();
		m_depthImage->TransitionImageLayout(cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
		Renderer::Get().EndSingleTimeCommands(cmd);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreatePipeline(VkExtent2D _extent) {
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
		std::vector < VkVertexInputBindingDescription > bindingDescription = Vertex::GetBindingDescription();
		std::vector < VkVertexInputAttributeDescription > attributeDescriptions = Vertex::GetAttributeDescriptions();

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
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
			m_sceneDescriptor->GetLayout()
			, m_texturesDescriptor->GetLayout()
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
	void ForwardPipeline::DeleteDepthRessources() {
		delete m_depthImageView;
		delete m_depthImage;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::DeletePipeline() {
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
	void ForwardPipeline::DeleteDescriptors() {
		delete m_texturesDescriptor;
		m_texturesDescriptor = nullptr;
		delete m_sceneDescriptor;
		m_sceneDescriptor = nullptr;
	}
}