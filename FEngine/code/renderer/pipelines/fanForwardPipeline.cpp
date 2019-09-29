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
	ForwardPipeline::ForwardPipeline(Device& _device, const VkExtent2D _extent ) :
		Pipeline(_device)
		, m_sceneDescriptor(nullptr){

		m_sampler = new Sampler(_device);
		m_sampler->CreateSampler(0, 8);
		CreateDepthRessources( _extent );

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if ( minUboAlignment > 0 ) {
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		dynamicUniformsVert.Resize( s_maximumNumModels * dynamicAlignmentVert, dynamicAlignmentVert );
		dynamicUniformsMaterial.Resize( s_maximumNumModels * dynamicAlignmentFrag, dynamicAlignmentFrag );

		for ( int uniformIndex = 0; uniformIndex < s_maximumNumModels; uniformIndex++ ) {
			dynamicUniformsMaterial[uniformIndex].color = glm::vec3( 1 );
			dynamicUniformsMaterial[uniformIndex].textureIndex = 0;
			dynamicUniformsMaterial[uniformIndex].shininess = 1;
		}

		CreateSceneDescriptor();
		CreateTextureDescriptor();
	}

	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::~ForwardPipeline() {
		DeleteDepthRessources();
		delete m_texturesDescriptor;
		delete m_sceneDescriptor;
		delete m_sampler;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Resize( const VkExtent2D _extent) {
		Pipeline::Resize(_extent);

 		DeleteDepthRessources();
		CreateDepthRessources( _extent );

		delete m_texturesDescriptor;
		m_texturesDescriptor = nullptr;
		CreateTextureDescriptor();
		DeletePipeline();
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreateSceneDescriptor() {
		delete m_sceneDescriptor;
		m_sceneDescriptor = new Descriptor( m_device );
		m_sceneDescriptor->AddUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		m_sceneDescriptor->AddDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, dynamicUniformsVert.GetTotalSize(), dynamicUniformsVert.GetAlignment() );
		m_sceneDescriptor->AddUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_sceneDescriptor->AddDynamicUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, dynamicUniformsMaterial.GetTotalSize(), dynamicUniformsMaterial.GetAlignment() );
		m_sceneDescriptor->AddUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		return m_sceneDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreateTextureDescriptor() {
		delete m_texturesDescriptor;
		m_texturesDescriptor = new  Descriptor( m_device );
		std::vector< Texture * > & textures = Renderer::Get().GetRessourceManager()->GetTextures();
		std::vector< VkImageView > imageViews( textures .size());
		for (int textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
			imageViews[textureIndex] = textures[textureIndex]->GetImageView();
		}
		m_texturesDescriptor->AddImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, imageViews, m_sampler->GetSampler() );

		return m_texturesDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::UpdateUniformBuffers() {
		m_sceneDescriptor->SetBinding( 0, &vertUniforms, sizeof( VertUniforms ) );
		m_sceneDescriptor->SetBinding( 1, &dynamicUniformsVert[0], dynamicUniformsVert.GetTotalSize() );
		m_sceneDescriptor->SetBinding( 2,  &fragUniforms, sizeof( FragUniforms ) );
		m_sceneDescriptor->SetBinding( 3, &dynamicUniformsMaterial[0], dynamicUniformsMaterial.GetTotalSize() );
		m_sceneDescriptor->SetBinding( 4, &lightUniforms, sizeof( LightsUniforms ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkImageView	ForwardPipeline::GetDepthImageView() {
		return m_depthImageView->GetImageView();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Bind( VkCommandBuffer _commandBuffer ) {
		Pipeline::Bind( _commandBuffer );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const uint32_t _indexOffset ) {
		
		std::vector<VkDescriptorSet> descriptors = {
			m_sceneDescriptor->GetSet()
			, m_texturesDescriptor->GetSet()
		}; 
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset  * static_cast<uint32_t>( dynamicUniformsVert.GetAlignment() )
			,_indexOffset  * static_cast<uint32_t>( dynamicUniformsMaterial.GetAlignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
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
	void ForwardPipeline::ConfigurePipeline() {
		m_bindingDescription = Vertex::GetBindingDescription();
		m_attributeDescriptions = Vertex::GetAttributeDescriptions();
		m_descriptorSetLayouts = {
			m_sceneDescriptor->GetLayout()
			, m_texturesDescriptor->GetLayout()
		};
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::DeleteDepthRessources() {
		delete m_depthImageView;
		delete m_depthImage;
	}
}