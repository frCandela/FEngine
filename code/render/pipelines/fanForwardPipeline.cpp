#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanImage.hpp"
#include "render/core/fanImageView.hpp" 
#include "render/core/fanBuffer.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanSampler.hpp"
#include "render/descriptors/fanDescriptor.hpp"
#include "render/descriptors/fanDescriptorTexture.hpp"
#include "render/descriptors/fanDescriptorSampler.hpp"
#include "render/util/fanVertex.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::ForwardPipeline( Device& _device, DescriptorTextures*& _textures, DescriptorSampler*& _sampler ) :
		Pipeline( _device )
		, m_sceneDescriptor( nullptr )
		, m_textures( _textures )
		, m_sampler( _sampler )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)m_device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if ( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}

		m_dynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
		m_dynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

		m_dynamicUniformsVert.Resize( 256 );
		m_dynamicUniformsMaterial.Resize( 256 );

		for ( int uniformIndex = 0; uniformIndex < m_dynamicUniformsMaterial.Size(); uniformIndex++ )
		{
			m_dynamicUniformsMaterial[ uniformIndex ].color = glm::vec4( 1 );
			m_dynamicUniformsMaterial[ uniformIndex ].shininess = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::~ForwardPipeline()
	{
		delete m_sceneDescriptor;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Resize( const VkExtent2D _extent )
	{
		Pipeline::Resize( _extent );
		DeletePipeline();
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::CreateDescriptors( const size_t _numSwapchainImages )
	{
		Debug::Log() << "Forward pipeline : create descriptors" << Debug::Endl();
		m_sceneDescriptor = new Descriptor( m_device, _numSwapchainImages );
		m_sceneDescriptor->SetUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		m_sceneDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment() );
		m_sceneDescriptor->SetUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_sceneDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, m_dynamicUniformsMaterial.Size(), m_dynamicUniformsMaterial.Alignment() );
		m_sceneDescriptor->SetUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		m_sceneDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ResizeDynamicDescriptors( const size_t _newSize )
	{
		m_dynamicUniformsVert.Resize( _newSize );
		m_dynamicUniformsMaterial.Resize( _newSize );
		m_sceneDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment(), 1 );
		m_sceneDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, m_dynamicUniformsMaterial.Size(), m_dynamicUniformsMaterial.Alignment(), 3 );
		m_sceneDescriptor->Update();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ReloadShaders()
	{
		Pipeline::ReloadShaders();

	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::UpdateUniformBuffers( const size_t _index )
	{
		m_sceneDescriptor->SetBinding( 0, _index, &m_vertUniforms, sizeof( VertUniforms ), 0 );
		m_sceneDescriptor->SetBinding( 1, _index, &m_dynamicUniformsVert[ 0 ], m_dynamicUniformsVert.Alignment() * m_dynamicUniformsVert.Size(), 0 );
		m_sceneDescriptor->SetBinding( 2, _index, &m_fragUniforms, sizeof( FragUniforms ), 0 );
		m_sceneDescriptor->SetBinding( 3, _index, &m_dynamicUniformsMaterial[ 0 ], m_dynamicUniformsMaterial.Alignment() * m_dynamicUniformsMaterial.Size(), 0 );
		m_sceneDescriptor->SetBinding( 4, _index, &m_lightUniforms, sizeof( LightsUniforms ), 0 );
	}



	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{

		std::vector<VkDescriptorSet> descriptors = {
			m_sceneDescriptor->GetSet( _indexFrame )
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast< uint32_t >( m_dynamicUniformsVert.Alignment() )
			,_indexOffset* static_cast< uint32_t >( m_dynamicUniformsMaterial.Alignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast< uint32_t >( descriptors.size() ),
			descriptors.data(),
			static_cast< uint32_t >( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ConfigurePipeline()
	{
		m_bindingDescription = Vertex::GetBindingDescription();
		m_attributeDescriptions = Vertex::GetAttributeDescriptions();
		m_descriptorSetLayouts = {
			m_sceneDescriptor->GetLayout()
			, m_textures->GetLayout()
			, m_sampler->GetLayout()
		};
	}
}