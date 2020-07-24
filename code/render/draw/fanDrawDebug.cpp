#include "render/draw/fanDrawDebug.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanFrameBuffer.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::Create( Device& _device, uint32_t _imagesCount )
	{
		mVertexBuffersLines.resize( _imagesCount );
		mVertexBuffersLinesNDT.resize( _imagesCount );
		mVertexBuffersTriangles.resize( _imagesCount );

		mDescriptorLines.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( UniformsDebug ) );
		mDescriptorLines.Create( _device, _imagesCount );

		mDescriptorLinesNDT.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( UniformsDebug ) );
		mDescriptorLinesNDT.Create( _device, _imagesCount );

		mDescriptorTriangles.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( UniformsDebug ) );
		mDescriptorTriangles.Create( _device, _imagesCount );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::Destroy( Device& _device )
	{
		mDescriptorLines.Destroy( _device );
		mDescriptorLinesNDT.Destroy( _device );
		mDescriptorTriangles.Destroy( _device );

		assert( (mVertexBuffersLines.size() == mVertexBuffersLinesNDT.size()) && (mVertexBuffersLines.size() == mVertexBuffersTriangles.size()) );
		for( int i = 0; i < mVertexBuffersLines.size(); i++ )
		{
			mVertexBuffersLines[i].Destroy( _device );
			mVertexBuffersLinesNDT[i].Destroy( _device );
			mVertexBuffersTriangles[i].Destroy( _device );
		} 
		mVertexBuffersLines.clear();
		mVertexBuffersLinesNDT.clear();
		mVertexBuffersTriangles.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		mDescriptorLines.SetData( _device, 0, _index, &mUniformsLines, sizeof( UniformsDebug ), 0 );
		mDescriptorLinesNDT.SetData( _device, 0, _index, &mUniformsLinesNDT, sizeof( UniformsDebug ), 0 );
		mDescriptorTriangles.SetData( _device, 0, _index, &mUniformsTriangles, sizeof( UniformsDebug ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::BindDescriptorsLines( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		mDescriptorLines.Bind( _commandBuffer, mPipelineLines.mPipelineLayout, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::BindDescriptorsLinesNDT( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		mDescriptorLinesNDT.Bind( _commandBuffer, mPipelineLinesNDT.mPipelineLayout, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::BindDescriptorsTriangles( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		mDescriptorTriangles.Bind( _commandBuffer, mPipelineTriangles.mPipelineLayout, _index );
	}	

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawDebug::GetPipelineConfigLines() const
	{
		PipelineConfig config( mVertexShaderLines, mFragmentShaderLines );
		
		config.bindingDescription = DebugVertex::GetBindingDescription();
		config.attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		config.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		config.depthStencilStateCreateInfo.depthTestEnable =  VK_TRUE;
		config.descriptorSetLayouts = { mDescriptorLines.mDescriptorSetLayout };
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		
		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawDebug::GetPipelineConfigLinesNDT() const
	{
		PipelineConfig config( mVertexShaderLinesNDT, mFragmentShaderLinesNDT );

		config.bindingDescription = DebugVertex::GetBindingDescription();
		config.attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		config.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		config.depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		config.descriptorSetLayouts = { mDescriptorLinesNDT.mDescriptorSetLayout };
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawDebug::GetPipelineConfigTriangles() const
	{
		PipelineConfig config( mVertexShaderTriangles, mFragmentShaderTriangles );

		config.bindingDescription = DebugVertex::GetBindingDescription();
		config.attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		config.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config.depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
		config.descriptorSetLayouts = { mDescriptorTriangles.mDescriptorSetLayout };
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent )
	{		
		if( !HasNothingToDraw() )
		{
			VkCommandBuffer commandBuffer = mCommandBuffers.mBuffers[_index];
			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( _renderPass.mRenderPass, _framebuffer.mFrameBuffers[_index] );
			VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

			if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
			{
				VkDeviceSize offsets[] = { 0 };
				if( mNumLines > 0 )
				{
					mPipelineLines.Bind( commandBuffer, _extent );
					BindDescriptorsLines( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { mVertexBuffersLines[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( mNumLines ), 1, 0, 0 );
				}
				if( mNumLinesNDT > 0 )
				{
					mPipelineLinesNDT.Bind( commandBuffer, _extent );
					BindDescriptorsLinesNDT( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { mVertexBuffersLinesNDT[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( mNumLinesNDT ), 1, 0, 0 );
				}
				if( mNumTriangles > 0 )
				{
					mPipelineTriangles.Bind( commandBuffer, _extent );
					BindDescriptorsTriangles( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { mVertexBuffersTriangles[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( mNumTriangles ), 1, 0, 0 );
				}
				if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
				{
					Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
				}
			}
			else
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << _index << "." << Debug::Endl();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::SetDebugDrawData( const uint32_t _index, Device& _device, const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		mNumLines = (int)_debugLines.size();
		mNumLinesNDT = (int)_debugLinesNoDepthTest.size();
		mNumTriangles = (int)_debugTriangles.size();		

		if( _debugLines.size() > 0 )
		{
			mVertexBuffersLines[_index].Destroy( _device );	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			mVertexBuffersLines[_index].Create(
				_device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			_device.AddDebugName( (uint64_t)mVertexBuffersLines[_index].mBuffer, "vertex buffers debug lines" );
			_device.AddDebugName( (uint64_t)mVertexBuffersLines[_index].mMemory, "vertex buffers debug lines" );

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					_device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _device, _debugLines.data(), size );
				VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mVertexBuffersLines[_index].mBuffer, size );
				_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( _device );
			}
		}

		if( _debugLinesNoDepthTest.size() > 0 )
		{
			mVertexBuffersLinesNDT[_index].Destroy( _device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			mVertexBuffersLinesNDT[_index].Create(
				_device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			_device.AddDebugName( (uint64_t)mVertexBuffersLinesNDT[_index].mBuffer, "vertex buffers debug lines NDT" );
			_device.AddDebugName( (uint64_t)mVertexBuffersLinesNDT[_index].mMemory, "vertex buffers debug lines NDT" );


			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					_device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _device, _debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mVertexBuffersLinesNDT[_index].mBuffer, size );
				_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( _device );
			}
		}

		if( _debugTriangles.size() > 0 )
		{
			mVertexBuffersTriangles[_index].Destroy( _device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			mVertexBuffersTriangles[_index].Create(
				_device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			_device.AddDebugName( (uint64_t)mVertexBuffersTriangles[_index].mBuffer, "vertex buffers debug triangles" );
			_device.AddDebugName( (uint64_t)mVertexBuffersTriangles[_index].mMemory, "vertex buffers debug triangles" );

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					_device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( _device, _debugTriangles.data(), size );
				VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mVertexBuffersTriangles[_index].mBuffer, size );
				_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( _device );
			}
		}
	}
}