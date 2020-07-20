#include "render/fanRenderer.hpp"

#include "core/fanDebug.hpp"
#include "core/input/fanInput.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/fanMeshManager.hpp"
#include "render/fanUIMesh.hpp"
#include "render/fanMesh.hpp"
#include "render/core/fanSwapChain.hpp"
#include "render/core/fanInstance.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanDevice.hpp"
#include "render/fanVertex.hpp"
#include "render/fanWindow.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer( Window& _window ) : mWindow( _window ), mDevice( _window.mDevice )
	{
		const uint32_t imagesCount = mWindow.mSwapchain.mImagesCount;

		Mesh::s_resourceManager.Init( mDevice );
		Texture::s_resourceManager.Init( mDevice );
		UIMesh::s_resourceManager.Init( mDevice );

		CreateRenderPasses();		

		mSamplerGameColor.Create( mDevice, 0, 1.f, VK_FILTER_LINEAR );
		mSamplerPostprocessColor.Create( mDevice, 0, 1.f, VK_FILTER_LINEAR );
		CreateFramebuffers( mWindow.mSwapchain.mExtent );
		mFramebuffersSwapchain.CreateForSwapchain( mDevice, imagesCount, mWindow.mSwapchain.mExtent, mRenderPassImgui, mWindow.mSwapchain.mImageViews );

		CreateTextureDescriptor();
		CreateShaders();

		mDrawDebug.Create( mDevice, imagesCount );
		mDrawUI.Create( mDevice, imagesCount );
		mDrawModels.Create( mDevice, imagesCount );	
		mDrawPostprocess.Create( mDevice, imagesCount, mImageViewGameColor );
		mDrawImgui.Create( mDevice, imagesCount, mRenderPassImgui.mRenderPass, mWindow.mWindow, mWindow.mSwapchain.mExtent, mImageViewPostprocessColor );

		CreatePipelines();
		CreateCommandBuffers();
		RecordAllCommandBuffers();	}

	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer()
	{
		vkDeviceWaitIdle( mDevice.mDevice );

		mDrawImgui.Destroy( mDevice );
		mDrawModels.Destroy( mDevice );
		mDrawUI.mPipeline.Destroy( mDevice );
		mDrawDebug.Destroy( mDevice );

		Mesh::s_resourceManager.Clear();
		Texture::s_resourceManager.Clear();
		UIMesh::s_resourceManager.Clear();

		mDrawUI.Destroy( mDevice );

		mDescriptorTextures.Destroy( mDevice );

		mDrawDebug.mPipelineLines.Destroy( mDevice );
		mDrawDebug.mPipelineLinesNDT.Destroy( mDevice );
		mDrawDebug.mPipelineTriangles.Destroy( mDevice );

		// game framebuffers & attachements
		mFrameBuffersGame.Destroy( mDevice );
		mImageGameDepth.Destroy( mDevice );
		mImageViewGameDepth.Destroy( mDevice );
		mSamplerGameColor.Destroy( mDevice );
		mImageGameColor.Destroy( mDevice );
		mImageViewGameColor.Destroy( mDevice );

		// pp frame buffers & attachements
		mFramebuffersPostprocess.Destroy( mDevice );
		mSamplerPostprocessColor.Destroy( mDevice );
		mImagePostprocessColor.Destroy( mDevice );
		mImageViewPostprocessColor.Destroy( mDevice );
		
		// render passes
		mRenderPassGame.Destroy( mDevice );
		mRenderPassPostprocess.Destroy( mDevice );
		mRenderPassImgui.Destroy( mDevice );

		mFramebuffersSwapchain.Destroy( mDevice );

		mDrawPostprocess.mPipeline.Destroy( mDevice );
		mDrawPostprocess.Destroy( mDevice );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreateShaders()
	{
		mDrawDebug.mVertexShaderLines.Create( mDevice, "code/shaders/debugLines.vert" );
		mDrawDebug.mFragmentShaderLines.Create( mDevice, "code/shaders/debugLines.frag" );
		mDrawDebug.mVertexShaderLinesNDT.Create( mDevice, "code/shaders/debugLines.vert" );
		mDrawDebug.mFragmentShaderLinesNDT.Create( mDevice, "code/shaders/debugLines.frag" );
		mDrawDebug.mVertexShaderTriangles.Create( mDevice, "code/shaders/debugTriangles.vert" );
		mDrawDebug.mFragmentShaderTriangles.Create( mDevice, "code/shaders/debugTriangles.frag" );
		mDrawModels.mVertexShader.Create( mDevice, "code/shaders/models.vert" );
		mDrawModels.mFragmentShader.Create( mDevice, "code/shaders/models.frag" );
		mDrawUI.mVertexShader.Create( mDevice, "code/shaders/ui.vert" );
		mDrawUI.mFragmentShader.Create( mDevice, "code/shaders/ui.frag" );
		mDrawPostprocess.mVertexShader.Create( mDevice, "code/shaders/postprocess.vert" );
		mDrawPostprocess.mFragmentShader.Create( mDevice, "code/shaders/postprocess.frag" );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyShaders()
	{
		mDrawDebug.mVertexShaderLines		.Destroy( mDevice );
		mDrawDebug.mFragmentShaderLines		.Destroy( mDevice );
		mDrawDebug.mVertexShaderLinesNDT	.Destroy( mDevice );
		mDrawDebug.mFragmentShaderLinesNDT	.Destroy( mDevice );
		mDrawDebug.mVertexShaderTriangles	.Destroy( mDevice );
		mDrawDebug.mFragmentShaderTriangles	.Destroy( mDevice );
		mDrawModels.mVertexShader			.Destroy( mDevice );
		mDrawModels.mFragmentShader			.Destroy( mDevice );
		mDrawUI.mVertexShader				.Destroy( mDevice );
		mDrawUI.mFragmentShader				.Destroy( mDevice );
		mDrawPostprocess.mVertexShader		.Destroy( mDevice );
		mDrawPostprocess.mFragmentShader	.Destroy( mDevice );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreatePipelines()
	{		
		const PipelineConfig debugLinesPipelineConfig = mDrawDebug.GetPipelineConfigLines();
		const PipelineConfig debugLinesNoDepthTestPipelineConfig = mDrawDebug.GetPipelineConfigLinesNDT();
		const PipelineConfig debugTrianglesPipelineConfig = mDrawDebug.GetPipelineConfigTriangles();
		const PipelineConfig ppPipelineConfig = mDrawPostprocess.GetPipelineConfig();
		const PipelineConfig modelsPipelineConfig = mDrawModels.GetPipelineConfig( mDescriptorTextures );
		const PipelineConfig uiPipelineConfig = mDrawUI.GetPipelineConfig( mDescriptorTextures );

		mDrawDebug.mPipelineLines.Create(		mDevice, debugLinesPipelineConfig,				mGameExtent, mRenderPassGame.mRenderPass );
		mDrawDebug.mPipelineLinesNDT.Create(	mDevice, debugLinesNoDepthTestPipelineConfig,	mGameExtent, mRenderPassGame.mRenderPass );
		mDrawDebug.mPipelineTriangles.Create(	mDevice, debugTrianglesPipelineConfig,			mGameExtent, mRenderPassGame.mRenderPass );
		mDrawModels.mPipeline.Create(			mDevice, modelsPipelineConfig, mGameExtent, mRenderPassGame.mRenderPass );
		mDrawUI.mPipeline.Create(				mDevice, uiPipelineConfig, mGameExtent,		mRenderPassPostprocess.mRenderPass );
		mDrawPostprocess.mPipeline.Create(		mDevice, ppPipelineConfig, mGameExtent,		mRenderPassPostprocess.mRenderPass );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyPipelines()
	{
		mDrawPostprocess.mPipeline.Destroy( mDevice );
		mDrawModels.mPipeline.Destroy( mDevice );
		mDrawUI.mPipeline.Destroy( mDevice );
		mDrawDebug.mPipelineLines.Destroy( mDevice );
		mDrawDebug.mPipelineLinesNDT.Destroy( mDevice );
		mDrawDebug.mPipelineTriangles.Destroy( mDevice );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame()
	{
		SCOPED_PROFILE( draw_frame );

		const VkResult result = mWindow.mSwapchain.AcquireNextImage( mDevice );
		if ( result == VK_ERROR_OUT_OF_DATE_KHR )
		{
			// window minimized
			if ( mWindow.GetExtent().width == 0 && mWindow.GetExtent().height == 0 )
			{
				glfwPollEvents();
				return;
			}
			else
			{
				ResizeSwapchain();
				return;
			}
		}
		else if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not acquire next image" );
		}
		else
		{
			vkWaitForFences( mDevice.mDevice, 1, mWindow.mSwapchain.GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max() );
			vkResetFences( mDevice.mDevice, 1, mWindow.mSwapchain.GetCurrentInFlightFence() );
		}

		ImGui::GetIO().DisplaySize = ImVec2( static_cast< float >( mWindow.mSwapchain.mExtent.width ), static_cast< float >( mWindow.mSwapchain.mExtent.height ) );

		if ( Texture::s_resourceManager.IsModified() )
		{
			WaitIdle();
			Debug::Log( "reload textures" );
			CreateTextureDescriptor();
			Texture::s_resourceManager.SetUnmodified();
		}

		const uint32_t currentFrame = mWindow.mSwapchain.mCurrentFrame;
		UpdateUniformBuffers( mDevice, currentFrame );
		{
		
			SCOPED_PROFILE( record_cmd );
			mDrawModels.RecordCommandBuffer( currentFrame, mRenderPassGame, mFrameBuffersGame, mGameExtent, mDescriptorTextures );
			mDrawDebug.RecordCommandBuffer( currentFrame, mRenderPassGame, mFrameBuffersGame, mGameExtent );
			mDrawUI.RecordCommandBuffer( currentFrame, mRenderPassPostprocess, mFramebuffersPostprocess, mGameExtent, mDescriptorTextures );
			mDrawImgui.RecordCommandBuffer( currentFrame, mDevice, mRenderPassImgui, mFramebuffersSwapchain );
			RecordPrimaryCommandBuffer( currentFrame );
		}

		{
			SCOPED_PROFILE( submit );
			SubmitCommandBuffers();
			mWindow.mSwapchain.PresentImage( mDevice );
			mWindow.mSwapchain.StartNextFrame();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeGame( btVector2 _newSize )
	{
		WaitIdle();
		const VkExtent2D extent = { ( uint32_t ) _newSize[ 0 ], ( uint32_t ) _newSize[ 1 ] };
		mGameExtent = extent;

		// game framebuffers & attachements
		mImageGameDepth.Destroy( mDevice );
		mImageViewGameDepth.Destroy( mDevice );
		mImageGameColor.Destroy( mDevice );
		mImageViewGameColor.Destroy( mDevice );
		mFrameBuffersGame.Destroy( mDevice );
		// postprocess framebuffers & attachements
		mImagePostprocessColor.Destroy( mDevice );
		mImageViewPostprocessColor.Destroy( mDevice );
		mFramebuffersPostprocess.Destroy( mDevice );
		CreateFramebuffers( extent );

		mDrawPostprocess.mDescriptorImage.Destroy( mDevice );
		mDrawPostprocess.mDescriptorImage.Create( mDevice, &mImageViewGameColor.mImageView, 1, &mDrawPostprocess.mSampler.mSampler );

		mDrawImgui.UpdateGameImageDescriptor( mDevice, mImageViewPostprocessColor );

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeSwapchain()
	{
		WaitIdle();
		const VkExtent2D extent = mWindow.GetExtent();
		Debug::Get() << Debug::Severity::highlight << "Resize renderer: " << extent.width << "x" << extent.height << Debug::Endl();
		mWindow.mSwapchain.Resize( mDevice , extent );

		mFramebuffersSwapchain.Destroy( mDevice );
		mFramebuffersSwapchain.CreateForSwapchain( mDevice, mWindow.mSwapchain.mImagesCount, extent, mRenderPassImgui, mWindow.mSwapchain.mImageViews );

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffers( Device& _device, const size_t _index )
	{		
		mDrawPostprocess.UpdateUniformBuffers( _device, _index );
		mDrawModels.UpdateUniformBuffers( _device, _index );
		mDrawDebug.UpdateUniformBuffers( _device, _index );
		mDrawUI.UpdateUniformBuffers( _device, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::WaitIdle()
	{
		vkDeviceWaitIdle( mDevice.mDevice );
		Debug::Log( "Renderer idle" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position )
	{
		mDrawModels.mUniforms.mUniformsProjView.view = _view;
		mDrawModels.mUniforms.mUniformsProjView.proj = _projection;
		mDrawModels.mUniforms.mUniformsProjView.proj[1][1] *= -1;

		mDrawModels.mUniforms.mUniformsCameraPosition.cameraPosition = _position;

		UniformsDebug * debugUniforms[3] = { &mDrawDebug.mUniformsLines, &mDrawDebug.mUniformsLinesNDT, &mDrawDebug.mUniformsTriangles };
		for( int i = 0; i < 3; i++ )
		{
			debugUniforms[i]->model = glm::mat4( 1.0 );
			debugUniforms[i]->view = _view;
			debugUniforms[i]->proj = mDrawModels.mUniforms.mUniformsProjView.proj;
			debugUniforms[i]->color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetDirectionalLights( const std::vector<RenderDataDirectionalLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumDirectionalLight );
		mDrawModels.mUniforms.mUniformsLights.dirLightsNum = (uint32_t)_lightData.size();
		for( int i = 0; i < _lightData.size(); ++i )
		{
			const RenderDataDirectionalLight& light = _lightData[i];
			mDrawModels.mUniforms.mUniformsLights.dirLights[i].direction = light.direction;
			mDrawModels.mUniforms.mUniformsLights.dirLights[i].ambiant = light.ambiant;
			mDrawModels.mUniforms.mUniformsLights.dirLights[i].diffuse = light.diffuse;
			mDrawModels.mUniforms.mUniformsLights.dirLights[i].specular = light.specular;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetPointLights( const std::vector<RenderDataPointLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumPointLights );
		mDrawModels.mUniforms.mUniformsLights.pointLightNum = (uint32_t)_lightData.size();
		for ( int i = 0; i < _lightData.size(); ++i )
		{
			const RenderDataPointLight& light = _lightData[i];
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].position	= light.position;
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].diffuse	= light.diffuse;
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].specular	= light.specular;
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].ambiant	= light.ambiant;
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].constant	= light.constant;
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].linear	= light.linear;
			mDrawModels.mUniforms.mUniformsLights.pointlights[i].quadratic = light.quadratic;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDrawData( const std::vector<RenderDataModel>& _drawData )
	{
		if ( _drawData.size() > mDrawModels.mDrawData.capacity() )
		{
			Debug::Warning( "Resizing draw data arrays" );
			WaitIdle();
			const size_t newSize = 2 * _drawData.size();
			const uint32_t imagesCount = mWindow.mSwapchain.mImagesCount;
			mDrawModels.mDrawData.reserve( newSize );
			mDrawModels.mUniforms.mDynamicUniformsMatrices.Resize( newSize );
			mDrawModels.mUniforms.mDynamicUniformsMaterial.Resize( newSize );
			mDrawModels.mDescriptorUniforms.ResizeDynamicUniformBinding( mDevice, imagesCount, mDrawModels.mUniforms.mDynamicUniformsMatrices.Size(), mDrawModels.mUniforms.mDynamicUniformsMatrices.Alignment(), 1 );
			mDrawModels.mDescriptorUniforms.ResizeDynamicUniformBinding( mDevice, imagesCount, mDrawModels.mUniforms.mDynamicUniformsMaterial.Size(), mDrawModels.mUniforms.mDynamicUniformsMaterial.Alignment(), 3 );
			mDrawModels.mDescriptorUniforms.UpdateDescriptorSets( mDevice, imagesCount );
		}

		mDrawModels.mDrawData.clear();
		for ( int dataIndex = 0; dataIndex < _drawData.size(); dataIndex++ )
		{
			const RenderDataModel& data = _drawData[ dataIndex ];

			if ( data.mesh != nullptr && !data.mesh->GetIndices().empty() )
			{
				// Transform
				mDrawModels.mUniforms.mDynamicUniformsMatrices[dataIndex].modelMat = data.modelMatrix;
				mDrawModels.mUniforms.mDynamicUniformsMatrices[dataIndex].normalMat = data.normalMatrix;

				// material
				mDrawModels.mUniforms.mDynamicUniformsMaterial[ dataIndex ].color = data.color;
				mDrawModels.mUniforms.mDynamicUniformsMaterial[ dataIndex ].shininess = data.shininess;

				// Mesh
				mDrawModels.mDrawData.push_back( { data.mesh, data.textureIndex } );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetUIDrawData( const std::vector<RenderDataUIMesh>& _drawData )
	{
		mDrawUI.mDrawData.resize( _drawData.size() );
		for ( int meshIndex = 0; meshIndex < _drawData.size(); meshIndex++ )
		{
			const RenderDataUIMesh& uiData = _drawData[ meshIndex ];

			mDrawUI.mDrawData[ meshIndex ].mesh = uiData.mesh;
			mDrawUI.mDrawData[ meshIndex ].textureIndex = uiData.textureIndex;
			mDrawUI.mUniforms.mDynamicUniforms[meshIndex].position = uiData.position;
			mDrawUI.mUniforms.mDynamicUniforms[meshIndex].scale = uiData.scale;
			mDrawUI.mUniforms.mDynamicUniforms[meshIndex].color = uiData.color;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		mDrawDebug.mNumLines = (int)_debugLines.size();
		mDrawDebug.mNumLinesNDT = (int)_debugLinesNoDepthTest.size();
		mDrawDebug.mNumTriangles = (int)_debugTriangles.size();

		const uint32_t currentFrame = mWindow.mSwapchain.mCurrentFrame;

		SCOPED_PROFILE( update_buffer );
		if( _debugLines.size() > 0 )
		{
			SCOPED_PROFILE( lines );
			mDrawDebug.mVertexBuffersLines[currentFrame].Destroy( mDevice );	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			mDrawDebug.mVertexBuffersLines[currentFrame].Create(
				mDevice,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					mDevice,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( mDevice, _debugLines.data(), size );
				VkCommandBuffer cmd = mDevice.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mDrawDebug.mVertexBuffersLines[currentFrame].mBuffer, size );
				mDevice.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( mDevice );
			}
		}

		if( _debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth );
			mDrawDebug.mVertexBuffersLinesNDT[currentFrame].Destroy( mDevice );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			mDrawDebug.mVertexBuffersLinesNDT[currentFrame].Create(
				mDevice,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					mDevice,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( mDevice, _debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = mDevice.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mDrawDebug.mVertexBuffersLinesNDT[currentFrame].mBuffer, size );
				mDevice.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( mDevice );
			}
		}

		if( _debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles );
			mDrawDebug.mVertexBuffersTriangles[currentFrame].Destroy( mDevice );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			mDrawDebug.mVertexBuffersTriangles[currentFrame].Create(
				mDevice,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					mDevice,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( mDevice, _debugTriangles.data(), size );
				VkCommandBuffer cmd = mDevice.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mDrawDebug.mVertexBuffersTriangles[currentFrame].mBuffer, size );
				mDevice.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( mDevice );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordAllCommandBuffers()
	{
		for( size_t i = 0; i < mWindow.mSwapchain.mImagesCount; i++ )
		{
			mDrawImgui.RecordCommandBuffer( i, mDevice, mRenderPassImgui, mFramebuffersSwapchain );
			mDrawModels.RecordCommandBuffer( i, mRenderPassGame, mFrameBuffersGame, mGameExtent, mDescriptorTextures );
			mDrawDebug.RecordCommandBuffer( i, mRenderPassGame, mFrameBuffersGame, mGameExtent );
			mDrawPostprocess.RecordCommandBuffer( i, mRenderPassPostprocess, mFramebuffersPostprocess, mGameExtent );
			mDrawUI.RecordCommandBuffer( i, mRenderPassPostprocess, mFramebuffersPostprocess, mGameExtent, mDescriptorTextures );
			RecordPrimaryCommandBuffer( i );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordPrimaryCommandBuffer( const size_t _index )
	{
		SCOPED_PROFILE( primary );
		VkCommandBuffer commandBuffer = mPrimaryCommandBuffers.mBuffers[ _index ];

		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( VK_NULL_HANDLE );

		std::vector<VkClearValue> clearValues( 2 );
		clearValues[ 0 ].color = { mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo =			  RenderPass::GetBeginInfo(	mRenderPassGame.mRenderPass,		mFrameBuffersGame.mFrameBuffers[_index],		  mGameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoPostprocess = RenderPass::GetBeginInfo( mRenderPassPostprocess.mRenderPass,mFramebuffersPostprocess.mFrameBuffers[_index],  mGameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoImGui =		  RenderPass::GetBeginInfo( mRenderPassImgui.mRenderPass,		mFramebuffersSwapchain.mFrameBuffers[_index],	  mWindow.mSwapchain.mExtent, clearValues.data(), (uint32_t)clearValues.size() );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &mDrawModels.mCommandBuffers.mBuffers[ _index ] );
				if ( !mDrawDebug.HasNothingToDraw() )
				{
					vkCmdExecuteCommands( commandBuffer, 1, &mDrawDebug.mCommandBuffers.mBuffers[_index] );
				}
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &mDrawPostprocess.mCommandBuffers.mBuffers[ _index ] );
				vkCmdExecuteCommands( commandBuffer, 1, &mDrawUI.mCommandBuffers.mBuffers[ _index ] );
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoImGui, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &mDrawImgui.mCommandBuffers.mBuffers[ _index ] );
			} vkCmdEndRenderPass( commandBuffer );


			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
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
	void Renderer::SubmitCommandBuffers()
	{
		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = mWindow.mSwapchain.GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mPrimaryCommandBuffers.mBuffers[mWindow.mSwapchain.mCurrentImageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = mWindow.mSwapchain.GetCurrentRenderFinishedSemaphore();

		VkResult result = vkQueueSubmit( mDevice.mGraphicsQueue, 1, &submitInfo, *mWindow.mSwapchain.GetCurrentInFlightFence() );
		if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not submit draw command buffer " );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders()
	{
		Debug::Highlight( "Reloading shaders" );

		vkDeviceWaitIdle( mDevice.mDevice );
		CreateTextureDescriptor();
		DestroyPipelines();
		DestroyShaders();
		CreateShaders();
		CreatePipelines();
		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadIcons()
	{
		WaitIdle();
		mDrawImgui.ReloadIcons( mDevice );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateCommandBuffers()
	{
		const uint32_t count = mWindow.mSwapchain.mImagesCount;
		mPrimaryCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY );

		mDrawModels.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawImgui.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawUI.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawPostprocess.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawDebug.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateRenderPasses()
	{
		// game
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( mWindow.mSwapchain.mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkAttachmentDescription depthAtt = RenderPass::GetDepthAttachment( mDevice.FindDepthFormat() );
			VkAttachmentReference	depthAttRef = RenderPass::GetDepthAttachmentReference( 1 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, &depthAttRef );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			
			VkAttachmentDescription attachmentDescriptions[2] = { colorAtt, depthAtt };
			mRenderPassGame.Create( mDevice, attachmentDescriptions, 2, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// postprocess
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( mWindow.mSwapchain.mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			mRenderPassPostprocess.Create( mDevice, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// imgui
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( mWindow.mSwapchain.mSurfaceFormat.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			mRenderPassImgui.Create( mDevice, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateFramebuffers( const VkExtent2D _extent )
	{	
		const VkFormat depthFormat = mDevice.FindDepthFormat();
		const VkFormat colorFormat = mWindow.mSwapchain.mSurfaceFormat.format;
		const uint32_t imagesCount = mWindow.mSwapchain.mImagesCount;

		// game color
		mImageGameColor.Create( mDevice, colorFormat, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		mImageViewGameColor.Create( mDevice, mImageGameColor.mImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		// game depth
		mImageGameDepth.Create( mDevice, depthFormat, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		mImageViewGameDepth.Create( mDevice, mImageGameDepth.mImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D );
		{
			VkCommandBuffer cmd = mDevice.BeginSingleTimeCommands();
			mImageGameDepth.TransitionImageLayout( cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1 );
			mDevice.EndSingleTimeCommands( cmd );
		}
		VkImageView gameViews[2] = { mImageViewGameColor.mImageView, mImageViewGameDepth.mImageView };
		mFrameBuffersGame.Create( mDevice, imagesCount, _extent, mRenderPassGame, gameViews, 2 );

		// pp color		
		mImagePostprocessColor.Create( mDevice, colorFormat, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		mImageViewPostprocessColor.Create( mDevice, mImagePostprocessColor.mImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		mFramebuffersPostprocess.Create( mDevice, imagesCount, _extent, mRenderPassPostprocess, &mImageViewPostprocessColor.mImageView, 1 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateTextureDescriptor()
	{		
		const std::vector< Texture* >& textures = Texture::s_resourceManager.GetList();
		std::vector< VkImageView > imageViews( textures.size() );
		for ( int i = 0; i < textures.size(); i++ )
		{
			imageViews[i] = textures[ i ]->mImageView;
		}
		mDescriptorTextures.Destroy( mDevice );
		mDescriptorTextures.Create( mDevice, imageViews.data(), static_cast<uint32_t>( imageViews.size() ) );
	}
}