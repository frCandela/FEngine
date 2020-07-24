#include "render/draw/fanDrawPostprocess.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanFrameBuffer.hpp"

namespace fan
{
 	//================================================================================================================================
	//================================================================================================================================
	void DrawPostprocess::Create( Device& _device, uint32_t _imagesCount, ImageView& _inputImageView )
	{
		CreateQuadVertexBuffer( _device );

		mSampler.Create( _device, 0, 0.f, VK_FILTER_NEAREST );
		
		mDescriptorUniform.Destroy( _device );
		mDescriptorUniform.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformsPostprocess ) );
		mDescriptorUniform.Create( _device, _imagesCount );
		mDescriptorUniform.SetData( _device, 0, 0, &mUniforms, sizeof( UniformsPostprocess ), 0 );
		
		mDescriptorImage.Create( _device, &_inputImageView.mImageView, 1, &mSampler.mSampler );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawPostprocess::Destroy( Device& _device )
	{
		mSampler.Destroy( _device );
		mDescriptorUniform.Destroy( _device );
		mDescriptorImage.Destroy( _device );
		mVertexBufferQuad.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawPostprocess::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		std::vector<VkDescriptorSet> descriptors = {
			mDescriptorImage.mDescriptorSets[0]
			, mDescriptorUniform.mDescriptorSets[_index]
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mPipeline.mPipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawPostprocess::GetPipelineConfig() const
	{
		PipelineConfig config( mVertexShader, mFragmentShader );

		config.bindingDescription.resize( 1 );
		config.bindingDescription[0].binding = 0;
		config.bindingDescription[0].stride = sizeof( glm::vec3 );
		config.bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		config.attributeDescriptions.resize( 1 );
		config.attributeDescriptions[0].binding = 0;
		config.attributeDescriptions[0].location = 0;
		config.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		config.attributeDescriptions[0].offset = 0;
		config.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		config.attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		config.attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		config.descriptorSetLayouts = { mDescriptorImage.mDescriptorSetLayout, mDescriptorUniform.mDescriptorSetLayout };

		return config;
	}
	//================================================================================================================================
	//================================================================================================================================
	void DrawPostprocess::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		mDescriptorUniform.SetData( _device, 0, _index, &mUniforms, sizeof( UniformsPostprocess ), 0 );
	}

	//================================================================================================================================
	// Used for postprocess
	//================================================================================================================================
	void DrawPostprocess::CreateQuadVertexBuffer( Device& _device )
	{
		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof( vertices[0] ) * vertices.size();
		mVertexBufferQuad.Create(
			_device,
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		_device.AddDebugName( (uint64_t)mVertexBufferQuad.mBuffer, "vertex buffer quad" );
		_device.AddDebugName( (uint64_t)mVertexBufferQuad.mMemory, "vertex buffer quad" );

		Buffer stagingBuffer;
		stagingBuffer.Create(
			_device,
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);		
		stagingBuffer.SetData( _device, vertices.data(), size );
		VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo( cmd, mVertexBufferQuad.mBuffer, size );
		_device.EndSingleTimeCommands( cmd );
		stagingBuffer.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawPostprocess::RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent )
	{
		VkCommandBuffer commandBuffer = mCommandBuffers.mBuffers[_index];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( _renderPass.mRenderPass, _framebuffer.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			mPipeline.Bind( commandBuffer, _extent );
			BindDescriptors( commandBuffer, _index );
			VkBuffer vertexBuffers[] = { mVertexBufferQuad.mBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
			vkCmdDraw( commandBuffer, static_cast<uint32_t>( 4 ), 1, 0, 0 );
			if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
		}
	}
}