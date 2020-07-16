#include "render/draw/fanDrawDebug.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void DrawDebug::Create( Device& _device, uint32_t _imagesCount )
	{
		mVertexBuffersLines.resize( _imagesCount );
		mVertexBuffersLinesNDT.resize( _imagesCount );
		mVertexBuffersTriangles.resize( _imagesCount );

		mDescriptorLines.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		mDescriptorLines.Create( _device, _imagesCount );

		mDescriptorLinesNDT.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		mDescriptorLinesNDT.Create( _device, _imagesCount );

		mDescriptorTriangles.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
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
		mDescriptorLines.SetData( _device, 0, _index, &mUniformsLines, sizeof( DebugUniforms ), 0 );
		mDescriptorLinesNDT.SetData( _device, 0, _index, &mUniformsLinesNDT, sizeof( DebugUniforms ), 0 );
		mDescriptorTriangles.SetData( _device, 0, _index, &mUniformsTriangles, sizeof( DebugUniforms ), 0 );
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
}