#include "render/draw/fanDrawModels.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"
#include "render/fanMesh.hpp"
#include "render/core/fanRenderPass.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::Create( Device& _device, uint32_t _imagesCount )
	{
		mSamplerTextures.Create( _device, 0, 8, VK_FILTER_LINEAR );
		mDescriptorSampler.Create( _device, mSamplerTextures.mSampler );

		const size_t initialDrawDataSize = 256;
		mDrawData.reserve( initialDrawDataSize );
		mUniforms.Create( _device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
		mUniforms.mDynamicUniformsMatrices.Resize( initialDrawDataSize );
		mUniforms.mDynamicUniformsMaterial.Resize( initialDrawDataSize );

		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( UniformViewProj ) );
		mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, mUniforms.mDynamicUniformsMatrices.Size(), mUniforms.mDynamicUniformsMatrices.Alignment() );
		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformCameraPosition ) );
		mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, mUniforms.mDynamicUniformsMaterial.Size(), mUniforms.mDynamicUniformsMaterial.Alignment() );
		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformLights ) );
		mDescriptorUniforms.Create( _device, _imagesCount );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::Destroy( Device& _device )
	{
		mPipeline.Destroy( _device );
		mDescriptorUniforms.Destroy( _device );
		mDescriptorSampler.Destroy( _device );
		mSamplerTextures.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawModels::GetPipelineConfig( DescriptorImages& _imagesDescriptor ) const
	{
		PipelineConfig config( mVertexShader, mFragmentShader );

		config.bindingDescription = Vertex::GetBindingDescription();
		config.attributeDescriptions = Vertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			mDescriptorUniforms.mDescriptorSetLayout
			, _imagesDescriptor.mDescriptorSetLayout
			, mDescriptorSampler.mDescriptorSetLayout
		};

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			mDescriptorUniforms.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast<uint32_t>( mUniforms.mDynamicUniformsMatrices.Alignment() )
			,_indexOffset* static_cast<uint32_t>( mUniforms.mDynamicUniformsMaterial.Alignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mPipeline.mPipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		mDescriptorUniforms.SetData( _device, 0, _index, &mUniforms.mUniformsProjView, sizeof( UniformViewProj ), 0 );
		mDescriptorUniforms.SetData( _device, 1, _index, &mUniforms.mDynamicUniformsMatrices[0], mUniforms.mDynamicUniformsMatrices.Alignment() * mUniforms.mDynamicUniformsMatrices.Size(), 0 );
		mDescriptorUniforms.SetData( _device, 2, _index, &mUniforms.mUniformsCameraPosition, sizeof( UniformCameraPosition ), 0 );
		mDescriptorUniforms.SetData( _device, 3, _index, &mUniforms.mDynamicUniformsMaterial[0], mUniforms.mDynamicUniformsMaterial.Alignment() * mUniforms.mDynamicUniformsMaterial.Size(), 0 );
		mDescriptorUniforms.SetData( _device, 4, _index, &mUniforms.mUniformsLights, sizeof( UniformLights ), 0 );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void UniformsModelDraw::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)_minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformMatrices );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformMatrices ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}

		mDynamicUniformsMatrices.SetAlignement( dynamicAlignmentVert );
		mDynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

		mDynamicUniformsMatrices.Resize( 256 );
		mDynamicUniformsMaterial.Resize( 256 );

		for( int uniformIndex = 0; uniformIndex < (int)mDynamicUniformsMaterial.Size(); uniformIndex++ )
		{
			mDynamicUniformsMaterial[uniformIndex].color = glm::vec4( 1 );
			mDynamicUniformsMaterial[uniformIndex].shininess = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent, DescriptorImages& _descriptorTextures )
	{		
		VkCommandBuffer commandBuffer = mCommandBuffers.mBuffers[_index];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( _renderPass.mRenderPass, _framebuffer.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			mPipeline.Bind( commandBuffer, _extent );

			for( uint32_t meshIndex = 0; meshIndex < mDrawData.size(); meshIndex++ )
			{
				DrawData& drawData = mDrawData[meshIndex];
				BindTexture( commandBuffer, drawData.textureIndex, mDescriptorSampler, _descriptorTextures, mPipeline.mPipelineLayout );
				BindDescriptors( commandBuffer, _index, meshIndex );
				VkDeviceSize offsets[] = { 0 };



				vkCmdBindVertexBuffers( commandBuffer, 0, 1, &drawData.mesh->GetVertexBuffer().mBuffer, offsets );
				vkCmdBindIndexBuffer( commandBuffer, drawData.mesh->GetIndexBuffer().mBuffer, 0, VK_INDEX_TYPE_UINT32 );
				vkCmdDrawIndexed( commandBuffer, static_cast<uint32_t>( drawData.mesh->GetIndices().size() ), 1, 0, 0, 0 );
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

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::SetDrawData( Device& _device, const uint32_t _imagesCount, const std::vector<RenderDataModel>& _drawData )
	{
		if( _drawData.size() > mDrawData.capacity() )
		{
			Debug::Warning( "Resizing draw data arrays" );
			vkDeviceWaitIdle( _device.mDevice );
			const size_t newSize = 2 * _drawData.size();
			mDrawData.reserve( newSize );
			mUniforms.mDynamicUniformsMatrices.Resize( newSize );
			mUniforms.mDynamicUniformsMaterial.Resize( newSize );
			mDescriptorUniforms.ResizeDynamicUniformBinding( _device, _imagesCount, mUniforms.mDynamicUniformsMatrices.Size(), mUniforms.mDynamicUniformsMatrices.Alignment(), 1 );
			mDescriptorUniforms.ResizeDynamicUniformBinding( _device, _imagesCount, mUniforms.mDynamicUniformsMaterial.Size(), mUniforms.mDynamicUniformsMaterial.Alignment(), 3 );
			mDescriptorUniforms.UpdateDescriptorSets( _device, _imagesCount );
		}

		mDrawData.clear();
		for( int dataIndex = 0; dataIndex < (int)_drawData.size(); dataIndex++ )
		{
			const RenderDataModel& data = _drawData[dataIndex];

			if( data.mesh != nullptr && !data.mesh->GetIndices().empty() )
			{
				// Transform
				mUniforms.mDynamicUniformsMatrices[dataIndex].modelMat = data.modelMatrix;
				mUniforms.mDynamicUniformsMatrices[dataIndex].normalMat = data.normalMatrix;

				// material
				mUniforms.mDynamicUniformsMaterial[dataIndex].color = data.color;
				mUniforms.mDynamicUniformsMaterial[dataIndex].shininess = data.shininess;

				// Mesh
				mDrawData.push_back( { data.mesh, data.textureIndex } );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::SetPointLights( const std::vector<RenderDataPointLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumPointLights );
		mUniforms.mUniformsLights.pointLightNum = (uint32_t)_lightData.size();
		for( int i = 0; i < (int)_lightData.size(); ++i )
		{
			const RenderDataPointLight& light = _lightData[i];
			mUniforms.mUniformsLights.pointlights[i].position = light.position;
			mUniforms.mUniformsLights.pointlights[i].diffuse = light.diffuse;
			mUniforms.mUniformsLights.pointlights[i].specular = light.specular;
			mUniforms.mUniformsLights.pointlights[i].ambiant = light.ambiant;
			mUniforms.mUniformsLights.pointlights[i].constant = light.constant;
			mUniforms.mUniformsLights.pointlights[i].linear = light.linear;
			mUniforms.mUniformsLights.pointlights[i].quadratic = light.quadratic;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  DrawModels::SetDirectionalLights( const std::vector<RenderDataDirectionalLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumDirectionalLight );
		mUniforms.mUniformsLights.dirLightsNum = (uint32_t)_lightData.size();
		for( int i = 0; i < (int)_lightData.size(); ++i )
		{
			const RenderDataDirectionalLight& light = _lightData[i];
			mUniforms.mUniformsLights.dirLights[i].direction = light.direction;
			mUniforms.mUniformsLights.dirLights[i].ambiant = light.ambiant;
			mUniforms.mUniformsLights.dirLights[i].diffuse = light.diffuse;
			mUniforms.mUniformsLights.dirLights[i].specular = light.specular;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  DrawModels::BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler& _descriptorSampler, DescriptorImages& _descriptorTextures, VkPipelineLayout _pipelineLayout )
	{
		assert( _textureIndex < Texture::s_resourceManager.GetList().size() );

		std::vector<VkDescriptorSet> descriptors = {
			 _descriptorTextures.mDescriptorSets[_textureIndex]
			 , _descriptorSampler.mDescriptorSet
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}
}