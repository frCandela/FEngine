#pragma once

#include "renderer/core/fanPipeline.h"

#include "renderer/fanUIMesh.h"

namespace fan
{
	class DescriptorTextures;
	class Sampler;

	//================================================================================================================================
	//================================================================================================================================
	class UIPipeline : public Pipeline
	{
	public:
		UIPipeline( Device& _device );
		~UIPipeline() override;

		void UpdateUniformBuffers( const size_t _index = 0 ) override;		
		void CreateDescriptors( const size_t _numSwapchainImages );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );

		UIMesh * m_testUiMesh;

	protected:
		void ConfigurePipeline() override;

	private:
		DescriptorTextures *	m_descriptorImageSampler = nullptr;
		Sampler *				m_sampler;
	};
}