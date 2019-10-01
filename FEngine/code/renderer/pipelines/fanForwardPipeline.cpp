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
	ForwardPipeline::ForwardPipeline(Device& _device ) :
		Pipeline(_device)
		, m_sceneDescriptor(nullptr){

		m_sampler = new Sampler( m_device );
		m_sampler->CreateSampler( 0, 8 );

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if ( minUboAlignment > 0 ) {
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		m_dynamicUniformsVert.Resize( GlobalValues::s_maximumNumModels * dynamicAlignmentVert, dynamicAlignmentVert );
		m_dynamicUniformsMaterial.Resize( GlobalValues::s_maximumNumModels * dynamicAlignmentFrag, dynamicAlignmentFrag );

		for ( int uniformIndex = 0; uniformIndex < GlobalValues::s_maximumNumModels; uniformIndex++ ) {
			m_dynamicUniformsMaterial[uniformIndex].color = glm::vec3( 1 );
			m_dynamicUniformsMaterial[uniformIndex].textureIndex = 0;
			m_dynamicUniformsMaterial[uniformIndex].shininess = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::~ForwardPipeline() {
		delete m_texturesDescriptor;
		delete m_sceneDescriptor;
		delete m_sampler;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Resize( const VkExtent2D _extent) {
		Pipeline::Resize(_extent);

		delete m_texturesDescriptor;
		m_texturesDescriptor = nullptr;
		CreateTextureDescriptor();
		DeletePipeline();
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::CreateDescriptors( const size_t _numSwapchainImages,  RessourceManager * const _ressourceManager ) {
		m_ressourceManager = _ressourceManager;
		
		m_sceneDescriptor = new Descriptor( m_device, _numSwapchainImages );
		m_sceneDescriptor->SetUniformBinding		( VK_SHADER_STAGE_VERTEX_BIT,	sizeof( VertUniforms ) );
		m_sceneDescriptor->SetDynamicUniformBinding ( VK_SHADER_STAGE_VERTEX_BIT,	m_dynamicUniformsVert.GetTotalSize(), m_dynamicUniformsVert.GetAlignment() );
		m_sceneDescriptor->SetUniformBinding		( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_sceneDescriptor->SetDynamicUniformBinding ( VK_SHADER_STAGE_FRAGMENT_BIT, m_dynamicUniformsMaterial.GetTotalSize(), m_dynamicUniformsMaterial.GetAlignment() );
		m_sceneDescriptor->SetUniformBinding		( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		m_sceneDescriptor->Create();

		CreateTextureDescriptor();

	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreateTextureDescriptor() {
		delete m_texturesDescriptor;
		m_texturesDescriptor = new  Descriptor( m_device, 1 );
		SetTextureDescriptor();

		return m_texturesDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ReloadShaders() {
		Pipeline::ReloadShaders();
		SetTextureDescriptor(0);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::SetTextureDescriptor( const int _index ) {
		const std::vector< Texture * > & texture = m_ressourceManager->GetTextures();

		std::vector< VkImageView > imageViews( texture.size() );
		for ( int textureIndex = 0; textureIndex < texture.size(); textureIndex++ ) {
			imageViews[textureIndex] = texture[textureIndex]->GetImageView();
		}
		m_texturesDescriptor->SetImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, imageViews, m_sampler->GetSampler(), _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::UpdateUniformBuffers( const size_t _index ) {
		m_sceneDescriptor->SetBinding( 0, _index, &m_vertUniforms,				sizeof( VertUniforms ),					0 );
		m_sceneDescriptor->SetBinding( 1, _index, &m_dynamicUniformsVert[0],		m_dynamicUniformsVert.GetTotalSize(),		0 );
		m_sceneDescriptor->SetBinding( 2, _index, &m_fragUniforms,				sizeof( FragUniforms ),					0 );
		m_sceneDescriptor->SetBinding( 3, _index, &m_dynamicUniformsMaterial[0],	m_dynamicUniformsMaterial.GetTotalSize(), 0 );
		m_sceneDescriptor->SetBinding( 4, _index, &m_lightUniforms,				sizeof( LightsUniforms ),				0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset ) {
		
		std::vector<VkDescriptorSet> descriptors = {
			m_sceneDescriptor->GetSet( _indexFrame )
			, m_texturesDescriptor->GetSet()
		}; 
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset  * static_cast<uint32_t>( m_dynamicUniformsVert.GetAlignment() )
			,_indexOffset  * static_cast<uint32_t>( m_dynamicUniformsMaterial.GetAlignment() )
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
	void ForwardPipeline::ConfigurePipeline() {
		m_bindingDescription = Vertex::GetBindingDescription();
		m_attributeDescriptions = Vertex::GetAttributeDescriptions();
		m_descriptorSetLayouts = {
			m_sceneDescriptor->GetLayout()
			, m_texturesDescriptor->GetLayout()
		};
	}
}