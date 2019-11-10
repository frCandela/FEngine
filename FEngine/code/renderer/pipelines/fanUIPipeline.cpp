#include "fanGlobalIncludes.h"
#include "renderer/pipelines/fanUIPipeline.h"

#include "renderer/core/fanDescriptorTexture.h"
#include "renderer/core/fanSampler.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	UIPipeline::UIPipeline( Device& _device ) :
		Pipeline( _device )
	{
		m_sampler = new Sampler( m_device );
		m_sampler->CreateSampler( 0, 8 );

		m_testUiMesh = new UIMesh();
		std::vector<UIVertex> vertices = {
			 UIVertex(glm::vec2(0.f, -0.5f ))
			,UIVertex(glm::vec2(0.5f, 0.5f ))
			,UIVertex(glm::vec2(-0.5f, 0.5f ))
		};

		m_testUiMesh->LoadFromVertices(vertices);
		m_testUiMesh->GenerateBuffers(_device);
	}

	//================================================================================================================================
	//================================================================================================================================
	UIPipeline::~UIPipeline()
	{
		delete m_sampler;
		delete m_descriptorImageSampler;
		delete m_testUiMesh;
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::BindDescriptors( VkCommandBuffer /*_commandBuffer*/, const size_t /*_indexFrame*/, const uint32_t /*_indexOffset*/ )
	{

// 		std::vector<VkDescriptorSet> descriptors = {
// 			m_descriptorImageSampler->GetSet( 0 )
// 		};
// 		std::vector<uint32_t> dynamicOffsets = {};
// 		vkCmdBindDescriptorSets(
// 			_commandBuffer,
// 			VK_PIPELINE_BIND_POINT_GRAPHICS,
// 			m_pipelineLayout,
// 			0,
// 			static_cast<uint32_t>( descriptors.size() ),
// 			descriptors.data(),
// 			static_cast<uint32_t>( dynamicOffsets.size() ),
// 			dynamicOffsets.data()
// 		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::ConfigurePipeline()
	{
		m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;

		m_bindingDescription = UIVertex::GetBindingDescription();
		m_attributeDescriptions = UIVertex::GetAttributeDescriptions();
		
		//m_descriptorSetLayouts = { m_descriptorImageSampler->GetLayout()/*, m_descriptorUniforms->GetLayout()*/ };
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::UpdateUniformBuffers( const size_t /*_index*/ )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::CreateDescriptors( const size_t /*_numSwapchainImages*/ )
	{
		delete m_descriptorImageSampler;
		m_descriptorImageSampler = new DescriptorTextures( m_device, m_sampler->GetSampler(), 1 );
	}
}