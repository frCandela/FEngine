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
	Renderer::Renderer( Window& _window ) : m_window( _window ), m_device( _window.mDevice )
	{
		const uint32_t imagesCount = m_window.mSwapchain.mImagesCount;

		mClearColor = glm::vec4( 0.f, 0.f, 0.2f, 1.f );

		Mesh::s_resourceManager.Init( m_device );
		Texture::s_resourceManager.Init( m_device );
		UIMesh::s_resourceManager.Init( m_device );

		CreateRenderPasses();		

		m_gameColorSampler.Create( m_device, 0, 1.f, VK_FILTER_LINEAR );
		m_ppColorSampler.Create( m_device, 0, 1.f, VK_FILTER_LINEAR );
		CreateFramebuffers( m_window.mSwapchain.mExtent );
		m_swapchainFramebuffers.CreateForSwapchain( m_device, imagesCount, m_window.mSwapchain.mExtent, m_renderPassImgui, m_window.mSwapchain.mImageViews );

		m_samplerTextures.Create( m_device, 0, 8, VK_FILTER_LINEAR );
		m_samplerDescriptorTextures.Create( m_device, m_samplerTextures.mSampler );
		CreateTextureDescriptor();

		CreateShaders();

		mDrawDebug.Create( m_device, imagesCount );
		mDrawUI.Create( m_device, imagesCount );

		mDrawModels.Create( m_device, imagesCount );	

		mDrawPostprocess.Create( m_device, imagesCount, m_gameColorImageView );

		CreatePipelines();

		m_imguiPipeline.Create( m_device, imagesCount, m_renderPassImgui.mRenderPass, m_window.mWindow, m_window.mSwapchain.mExtent, m_ppColorImageView );

		CreateCommandBuffers();
		RecordAllCommandBuffers();	}

	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer()
	{
		vkDeviceWaitIdle( m_device.mDevice );

		m_imguiPipeline.Destroy( m_device );
		mDrawModels.Destroy( m_device );
		mDrawUI.mPipeline.Destroy( m_device );
		

		mDrawDebug.Destroy( m_device );

		Mesh::s_resourceManager.Clear();
		Texture::s_resourceManager.Clear();
		UIMesh::s_resourceManager.Clear();

		mDrawUI.Destroy( m_device );

		m_samplerDescriptorTextures.Destroy( m_device );		
		m_imagesDescriptor.Destroy( m_device );

		m_samplerTextures.Destroy( m_device );		

		mDrawDebug.mPipelineLines.Destroy( m_device );
		mDrawDebug.mPipelineLinesNDT.Destroy( m_device );
		mDrawDebug.mPipelineTriangles.Destroy( m_device );

		// game framebuffers & attachements
		m_gameFrameBuffers.Destroy( m_device );
		m_gameDepthImage.Destroy( m_device );
		m_gameDepthImageView.Destroy( m_device );
		m_gameColorSampler.Destroy( m_device );
		m_gameColorImage.Destroy( m_device );
		m_gameColorImageView.Destroy( m_device );

		// pp frame buffers & attachements
		m_ppFramebuffers.Destroy( m_device );
		m_ppColorSampler.Destroy( m_device );
		m_ppColorImage.Destroy( m_device );
		m_ppColorImageView.Destroy( m_device );
		
		// render passes
		m_renderPassGame.Destroy( m_device );
		m_renderPassPostprocess.Destroy( m_device );
		m_renderPassImgui.Destroy( m_device );

		m_swapchainFramebuffers.Destroy( m_device );

		mDrawPostprocess.mPipeline.Destroy( m_device );
		mDrawPostprocess.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreateShaders()
	{
		mDrawDebug.mVertexShaderLines.Create( m_device, "code/shaders/debugLines.vert" );
		mDrawDebug.mFragmentShaderLines.Create( m_device, "code/shaders/debugLines.frag" );
		mDrawDebug.mVertexShaderLinesNDT.Create( m_device, "code/shaders/debugLines.vert" );
		mDrawDebug.mFragmentShaderLinesNDT.Create( m_device, "code/shaders/debugLines.frag" );
		mDrawDebug.mVertexShaderTriangles.Create( m_device, "code/shaders/debugTriangles.vert" );
		mDrawDebug.mFragmentShaderTriangles.Create( m_device, "code/shaders/debugTriangles.frag" );
		mDrawModels.mVertexShader.Create( m_device, "code/shaders/forward.vert" );
		mDrawModels.mFragmentShader.Create( m_device, "code/shaders/forward.frag" );
		mDrawUI.mVertexShader.Create( m_device, "code/shaders/ui.vert" );
		mDrawUI.mFragmentShader.Create( m_device, "code/shaders/ui.frag" );
		mDrawPostprocess.mVertexShader.Create( m_device, "code/shaders/postprocess.vert" );
		mDrawPostprocess.mFragmentShader.Create( m_device, "code/shaders/postprocess.frag" );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyShaders()
	{
		mDrawDebug.mVertexShaderLines				.Destroy( m_device );
		mDrawDebug.mFragmentShaderLines				.Destroy( m_device );
		mDrawDebug.mVertexShaderLinesNDT	.Destroy( m_device );
		mDrawDebug.mFragmentShaderLinesNDT	.Destroy( m_device );
		mDrawDebug.mVertexShaderTriangles			.Destroy( m_device );
		mDrawDebug.mFragmentShaderTriangles			.Destroy( m_device );
		mDrawModels.mVertexShader	.Destroy( m_device );
		mDrawModels.mFragmentShader	.Destroy( m_device );
		mDrawUI.mVertexShader						.Destroy( m_device );
		mDrawUI.mFragmentShader						.Destroy( m_device );
		mDrawPostprocess.mVertexShader						.Destroy( m_device );
		mDrawPostprocess.mFragmentShader						.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreatePipelines()
	{		
		const PipelineConfig debugLinesPipelineConfig = mDrawDebug.GetPipelineConfigLines();
		const PipelineConfig debugLinesNoDepthTestPipelineConfig = mDrawDebug.GetPipelineConfigLinesNDT();
		const PipelineConfig debugTrianglesPipelineConfig = mDrawDebug.GetPipelineConfigTriangles();
		const PipelineConfig ppPipelineConfig = mDrawPostprocess.GetPipelineConfig();
		const PipelineConfig forwardPipelineConfig = mDrawModels.GetPipelineConfig( m_imagesDescriptor, m_samplerDescriptorTextures );
		const PipelineConfig uiPipelineConfig = mDrawUI.GetPipelineConfig( m_imagesDescriptor );

		mDrawDebug.mPipelineLines.Create( m_device, debugLinesPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		mDrawDebug.mPipelineLinesNDT.Create( m_device, debugLinesNoDepthTestPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		mDrawDebug.mPipelineTriangles.Create( m_device, debugTrianglesPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		mDrawModels.mPipeline.Create( m_device, forwardPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		mDrawUI.mPipeline.Create( m_device, uiPipelineConfig, m_gameExtent, m_renderPassPostprocess.mRenderPass );
		mDrawPostprocess.mPipeline.Create( m_device, ppPipelineConfig, m_gameExtent, m_renderPassPostprocess.mRenderPass );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyPipelines()
	{
		mDrawPostprocess.mPipeline.Destroy( m_device );
		mDrawModels.mPipeline.Destroy( m_device );
		mDrawUI.mPipeline.Destroy( m_device );
		mDrawDebug.mPipelineLines.Destroy( m_device );
		mDrawDebug.mPipelineLinesNDT.Destroy( m_device );
		mDrawDebug.mPipelineTriangles.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame()
	{
		SCOPED_PROFILE( draw_frame );

		const VkResult result = m_window.mSwapchain.AcquireNextImage( m_device );
		if ( result == VK_ERROR_OUT_OF_DATE_KHR )
		{

			// window minimized
			if ( m_window.GetExtent().width == 0 && m_window.GetExtent().height == 0 )
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
			vkWaitForFences( m_device.mDevice, 1, m_window.mSwapchain.GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max() );
			vkResetFences( m_device.mDevice, 1, m_window.mSwapchain.GetCurrentInFlightFence() );
		}

		ImGui::GetIO().DisplaySize = ImVec2( static_cast< float >( m_window.mSwapchain.mExtent.width ), static_cast< float >( m_window.mSwapchain.mExtent.height ) );

		if ( Texture::s_resourceManager.IsModified() )
		{
			WaitIdle();
			Debug::Log( "reload textures" );
			CreateTextureDescriptor();
			Texture::s_resourceManager.SetUnmodified();
		}

		const uint32_t currentFrame = m_window.mSwapchain.mCurrentFrame;
		UpdateUniformBuffers( m_device, currentFrame );
		{
			SCOPED_PROFILE( record_cmd );
			RecordCommandBufferForward( currentFrame );
			RecordCommandBufferDebug( currentFrame );
			RecordCommandBufferUI( currentFrame );
			RecordCommandBufferImgui( currentFrame );
			RecordPrimaryCommandBuffer( currentFrame );
		}

		{
			SCOPED_PROFILE( submit );
			SubmitCommandBuffers();
			m_window.mSwapchain.PresentImage( m_device );
			m_window.mSwapchain.StartNextFrame();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeGame( btVector2 _newSize )
	{
		WaitIdle();
		const VkExtent2D extent = { ( uint32_t ) _newSize[ 0 ], ( uint32_t ) _newSize[ 1 ] };
		m_gameExtent = extent;

		// game framebuffers & attachements
		m_gameDepthImage.Destroy( m_device );
		m_gameDepthImageView.Destroy( m_device );
		m_gameColorImage.Destroy( m_device );
		m_gameColorImageView.Destroy( m_device );
		m_gameFrameBuffers.Destroy( m_device );
		// postprocess framebuffers & attachements
		m_ppColorImage.Destroy( m_device );
		m_ppColorImageView.Destroy( m_device );
		m_ppFramebuffers.Destroy( m_device );
		CreateFramebuffers( extent );

		mDrawPostprocess.mDescriptorImage.Destroy( m_device );
		mDrawPostprocess.mDescriptorImage.Create( m_device, &m_gameColorImageView.mImageView, 1, mDrawPostprocess.mSampler.mSampler );

		m_imguiPipeline.UpdateGameImageDescriptor( m_device, m_ppColorImageView );

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeSwapchain()
	{
		WaitIdle();
		const VkExtent2D extent = m_window.GetExtent();
		Debug::Get() << Debug::Severity::highlight << "Resize renderer: " << extent.width << "x" << extent.height << Debug::Endl();
		m_window.mSwapchain.Resize( m_device , extent );

		m_swapchainFramebuffers.Destroy( m_device );
		m_swapchainFramebuffers.CreateForSwapchain( m_device, m_window.mSwapchain.mImagesCount, extent, m_renderPassImgui, m_window.mSwapchain.mImageViews );

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
		vkDeviceWaitIdle( m_device.mDevice );
		Debug::Log( "Renderer idle" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position )
	{
		mDrawModels.mUniforms.m_vertUniforms.view = _view;
		mDrawModels.mUniforms.m_vertUniforms.proj = _projection;
		mDrawModels.mUniforms.m_vertUniforms.proj[1][1] *= -1;

		mDrawModels.mUniforms.m_fragUniforms.cameraPosition = _position;

		DebugUniforms * debugUniforms[3] = { &mDrawDebug.mUniformsLines, &mDrawDebug.mUniformsLinesNDT, &mDrawDebug.mUniformsTriangles };
		for( int i = 0; i < 3; i++ )
		{
			debugUniforms[i]->model = glm::mat4( 1.0 );
			debugUniforms[i]->view = _view;
			debugUniforms[i]->proj = mDrawModels.mUniforms.m_vertUniforms.proj;
			debugUniforms[i]->color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetDirectionalLights( const std::vector<DrawDirectionalLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumDirectionalLight );
		mDrawModels.mUniforms.m_lightUniforms.dirLightsNum = (uint32_t)_lightData.size();
		for( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawDirectionalLight& light = _lightData[i];
			mDrawModels.mUniforms.m_lightUniforms.dirLights[i].direction = light.direction;
			mDrawModels.mUniforms.m_lightUniforms.dirLights[i].ambiant = light.ambiant;
			mDrawModels.mUniforms.m_lightUniforms.dirLights[i].diffuse = light.diffuse;
			mDrawModels.mUniforms.m_lightUniforms.dirLights[i].specular = light.specular;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetPointLights( const std::vector<DrawPointLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumPointLights );
		mDrawModels.mUniforms.m_lightUniforms.pointLightNum = (uint32_t)_lightData.size();
		for ( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawPointLight& light = _lightData[i];
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].position	= light.position;
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].diffuse	= light.diffuse;
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].specular	= light.specular;
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].ambiant	= light.ambiant;
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].constant	= light.constant;
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].linear	= light.linear;
			mDrawModels.mUniforms.m_lightUniforms.pointlights[i].quadratic = light.quadratic;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDrawData( const std::vector<DrawMesh>& _drawData )
	{
		if ( _drawData.size() > mDrawModels.mDrawData.capacity() )
		{
			Debug::Warning( "Resizing draw data arrays" );
			WaitIdle();
			const size_t newSize = 2 * _drawData.size();
			const uint32_t imagesCount = m_window.mSwapchain.mImagesCount;
			mDrawModels.mDrawData.reserve( newSize );
			mDrawModels.mUniforms.m_dynamicUniformsVert.Resize( newSize );
			mDrawModels.mUniforms.m_dynamicUniformsMaterial.Resize( newSize );
			mDrawModels.mDescriptorUniforms.ResizeDynamicUniformBinding( m_device, imagesCount, mDrawModels.mUniforms.m_dynamicUniformsVert.Size(), mDrawModels.mUniforms.m_dynamicUniformsVert.Alignment(), 1 );
			mDrawModels.mDescriptorUniforms.ResizeDynamicUniformBinding( m_device, imagesCount, mDrawModels.mUniforms.m_dynamicUniformsMaterial.Size(), mDrawModels.mUniforms.m_dynamicUniformsMaterial.Alignment(), 3 );
			mDrawModels.mDescriptorUniforms.UpdateDescriptorSets( m_device, imagesCount );
		}

		mDrawModels.mDrawData.clear();
		for ( int dataIndex = 0; dataIndex < _drawData.size(); dataIndex++ )
		{
			const DrawMesh& data = _drawData[ dataIndex ];

			if ( data.mesh != nullptr && !data.mesh->GetIndices().empty() )
			{
				// Transform
				mDrawModels.mUniforms.m_dynamicUniformsVert[dataIndex].modelMat = data.modelMatrix;
				mDrawModels.mUniforms.m_dynamicUniformsVert[dataIndex].normalMat = data.normalMatrix;

				// material
				mDrawModels.mUniforms.m_dynamicUniformsMaterial[ dataIndex ].color = data.color;
				mDrawModels.mUniforms.m_dynamicUniformsMaterial[ dataIndex ].shininess = data.shininess;

				// Mesh
				mDrawModels.mDrawData.push_back( { data.mesh, data.textureIndex } );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetUIDrawData( const std::vector<DrawUIMesh>& _drawData )
	{
		m_uiMeshDrawArray.resize( _drawData.size() );
		for ( int meshIndex = 0; meshIndex < _drawData.size(); meshIndex++ )
		{
			const DrawUIMesh& uiData = _drawData[ meshIndex ];

			m_uiMeshDrawArray[ meshIndex ].mesh = uiData.mesh;
			m_uiMeshDrawArray[ meshIndex ].textureIndex = uiData.textureIndex;
			mDrawUI.mUniforms.mUidynamicUniformsVert[meshIndex].position = uiData.position;
			mDrawUI.mUniforms.mUidynamicUniformsVert[meshIndex].scale = uiData.scale;
			mDrawUI.mUniforms.mUidynamicUniformsVert[meshIndex].color = uiData.color;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		mDrawDebug.mNumLines = (int)_debugLines.size();
		mDrawDebug.mNumLinesNDT = (int)_debugLinesNoDepthTest.size();
		mDrawDebug.mNumTriangles = (int)_debugTriangles.size();

		const uint32_t currentFrame = m_window.mSwapchain.mCurrentFrame;

		SCOPED_PROFILE( update_buffer );
		if( _debugLines.size() > 0 )
		{
			SCOPED_PROFILE( lines );
			mDrawDebug.mVertexBuffersLines[currentFrame].Destroy( m_device );	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			mDrawDebug.mVertexBuffersLines[currentFrame].Create(
				m_device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					m_device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( m_device, _debugLines.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mDrawDebug.mVertexBuffersLines[currentFrame].mBuffer, size );
				m_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( m_device );
			}
		}

		if( _debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth );
			mDrawDebug.mVertexBuffersLinesNDT[currentFrame].Destroy( m_device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			mDrawDebug.mVertexBuffersLinesNDT[currentFrame].Create(
				m_device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					m_device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( m_device, _debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mDrawDebug.mVertexBuffersLinesNDT[currentFrame].mBuffer, size );
				m_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( m_device );
			}
		}

		if( _debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles );
			mDrawDebug.mVertexBuffersTriangles[currentFrame].Destroy( m_device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			mDrawDebug.mVertexBuffersTriangles[currentFrame].Create(
				m_device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					m_device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( m_device, _debugTriangles.data(), size );
				VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, mDrawDebug.mVertexBuffersTriangles[currentFrame].mBuffer, size );
				m_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( m_device );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordAllCommandBuffers()
	{
		for( size_t i = 0; i < m_window.mSwapchain.mImagesCount; i++ )
		{
			RecordCommandBufferImgui( i );
			RecordCommandBufferForward( i );
			RecordCommandBufferDebug( i );
			RecordCommandBufferPostProcess( i );
			RecordCommandBufferUI( i );
			RecordPrimaryCommandBuffer( i );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordPrimaryCommandBuffer( const size_t _index )
	{
		SCOPED_PROFILE( primary );
		VkCommandBuffer commandBuffer = m_primaryCommandBuffers.mBuffers[ _index ];

		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( VK_NULL_HANDLE );

		std::vector<VkClearValue> clearValues( 2 );
		clearValues[ 0 ].color = { mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo =			  RenderPass::GetBeginInfo(	m_renderPassGame.mRenderPass,		m_gameFrameBuffers.mFrameBuffers[_index],		  m_gameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoPostprocess = RenderPass::GetBeginInfo( m_renderPassPostprocess.mRenderPass,m_ppFramebuffers.mFrameBuffers[_index],  m_gameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoImGui =		  RenderPass::GetBeginInfo( m_renderPassImgui.mRenderPass,		m_swapchainFramebuffers.mFrameBuffers[_index],	  m_window.mSwapchain.mExtent, clearValues.data(), (uint32_t)clearValues.size() );

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
				vkCmdExecuteCommands( commandBuffer, 1, &m_imguiCommandBuffers.mBuffers[ _index ] );
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
	void Renderer::RecordCommandBufferPostProcess( const size_t _index )
	{

		VkCommandBuffer commandBuffer = mDrawPostprocess.mCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassPostprocess.mRenderPass, m_ppFramebuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{			
			mDrawPostprocess.mPipeline.Bind( commandBuffer, m_gameExtent, _index );
			mDrawPostprocess.BindDescriptors( commandBuffer, _index );
			VkBuffer vertexBuffers[] = { mDrawPostprocess.mVertexBufferQuad.mBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
			vkCmdDraw( commandBuffer, static_cast< uint32_t >( 4 ), 1, 0, 0 );
			if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
			{
				Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
			}
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "Could not record command buffer " << commandBuffer << "." << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferUI( const size_t _index )
	{
		VkCommandBuffer commandBuffer = mDrawUI.mCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassPostprocess.mRenderPass, m_ppFramebuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			mDrawUI.mPipeline.Bind( commandBuffer, m_gameExtent, _index );
			//m_uiPipeline.BindDescriptors( commandBuffer, _index, 0 );

			VkDeviceSize offsets[] = { 0 };

			for ( uint32_t meshIndex = 0; meshIndex < m_uiMeshDrawArray.size(); meshIndex++ )
			{
				UIDrawData drawData = m_uiMeshDrawArray[ meshIndex ];
				UIMesh* mesh = drawData.mesh;
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer().mBuffer };
				mDrawUI.BindDescriptors( commandBuffer, _index, meshIndex );
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				BindTexture( commandBuffer, drawData.textureIndex, &mDrawUI.mDescriptorSampler, mDrawUI.mPipeline.mPipelineLayout );
				vkCmdDraw( commandBuffer, static_cast< uint32_t >( mesh->GetVertices().size() ), 1, 0, 0 );
			}

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
	void Renderer::RecordCommandBufferImgui( const size_t _index )
	{
		SCOPED_PROFILE( imgui );
		m_imguiPipeline.UpdateBuffer( m_device, _index );

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassImgui.mRenderPass, m_swapchainFramebuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_imguiPipeline.DrawFrame( commandBuffer, _index );

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
	void Renderer::RecordCommandBufferForward( const size_t _index )
	{
		SCOPED_PROFILE( geometry );
		VkCommandBuffer commandBuffer = mDrawModels.mCommandBuffers.mBuffers[_index];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassGame.mRenderPass, m_gameFrameBuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			mDrawModels.mPipeline.Bind( commandBuffer, m_gameExtent, _index );

			for( uint32_t meshIndex = 0; meshIndex < mDrawModels.mDrawData.size(); meshIndex++ )
			{
				DrawData& drawData = mDrawModels.mDrawData[meshIndex];
				BindTexture( commandBuffer, drawData.textureIndex, &m_samplerDescriptorTextures, mDrawModels.mPipeline.mPipelineLayout );
				mDrawModels.BindDescriptors( commandBuffer, _index, meshIndex );
				VkDeviceSize offsets[] = { 0 };
				VkBuffer vertexBuffers[] = { drawData.mesh->GetVertexBuffer().mBuffer };
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
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
	void Renderer::RecordCommandBufferDebug( const size_t _index )
	{
		SCOPED_PROFILE( debug );
		if( !mDrawDebug.HasNothingToDraw() )
		{
			VkCommandBuffer commandBuffer = mDrawDebug.mCommandBuffers.mBuffers[_index];
			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassGame.mRenderPass, m_gameFrameBuffers.mFrameBuffers[_index] );
			VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

			if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
			{
				VkDeviceSize offsets[] = { 0 };
				if( mDrawDebug.mNumLines > 0 )
				{
					mDrawDebug.mPipelineLines.Bind( commandBuffer, m_gameExtent, _index );
					mDrawDebug.BindDescriptorsLines( commandBuffer, _index );					
					VkBuffer vertexBuffers[] = { mDrawDebug.mVertexBuffersLines[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( mDrawDebug.mNumLines ), 1, 0, 0 );
				}
				if( mDrawDebug.mNumLinesNDT > 0 )
				{
					mDrawDebug.mPipelineLinesNDT.Bind( commandBuffer, m_gameExtent, _index );
					mDrawDebug.BindDescriptorsLinesNDT( commandBuffer, _index );					
					VkBuffer vertexBuffers[] = { mDrawDebug.mVertexBuffersLinesNDT[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( mDrawDebug.mNumLinesNDT ), 1, 0, 0 );
				}
				if( mDrawDebug.mNumTriangles > 0 )
				{
					mDrawDebug.mPipelineTriangles.Bind( commandBuffer, m_gameExtent, _index );
					mDrawDebug.BindDescriptorsTriangles( commandBuffer, _index );
					VkBuffer vertexBuffers[] = { mDrawDebug.mVertexBuffersTriangles[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( mDrawDebug.mNumTriangles ), 1, 0, 0 );
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
	bool Renderer::SubmitCommandBuffers()
	{
		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		std::vector< VkCommandBuffer> commandBuffers = {
			m_primaryCommandBuffers.mBuffers[ m_window.mSwapchain.mCurrentImageIndex ]
			//, m_imguiCommandBuffers[m_window.mSwapchain.GetCurrentImageIndex()]
		};

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = m_window.mSwapchain.GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount = static_cast< uint32_t >( commandBuffers.size() );
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = m_window.mSwapchain.GetCurrentRenderFinishedSemaphore();

		VkResult result = vkQueueSubmit( m_device.mGraphicsQueue, 1, &submitInfo, *m_window.mSwapchain.GetCurrentInFlightFence() );
		if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not submit draw command buffer " );
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler* _samplerDescriptor, VkPipelineLayout _pipelineLayout )
	{
		assert( _textureIndex < Texture::s_resourceManager.GetList().size() );

		std::vector<VkDescriptorSet> descriptors = {
			 m_imagesDescriptor.mDescriptorSets[_textureIndex]
			 , _samplerDescriptor->mDescriptorSet
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

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders()
	{
		Debug::Highlight( "Reloading shaders" );

		vkDeviceWaitIdle( m_device.mDevice );
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
		m_imguiPipeline.ReloadIcons( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateCommandBuffers()
	{
		const uint32_t count = m_window.mSwapchain.mImagesCount;
		m_primaryCommandBuffers.Create( m_device, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		mDrawModels.mCommandBuffers.Create( m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		m_imguiCommandBuffers.Create( m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawUI.mCommandBuffers.Create( m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawPostprocess.mCommandBuffers.Create( m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		mDrawDebug.mCommandBuffers.Create( m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPasses()
	{
		bool result = true;

		// game
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.mSwapchain.mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkAttachmentDescription depthAtt = RenderPass::GetDepthAttachment( m_device.FindDepthFormat() );
			VkAttachmentReference	depthAttRef = RenderPass::GetDepthAttachmentReference( 1 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, &depthAttRef );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			
			VkAttachmentDescription attachmentDescriptions[2] = { colorAtt, depthAtt };
			result &= m_renderPassGame.Create( m_device, attachmentDescriptions, 2, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// postprocess
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.mSwapchain.mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassPostprocess.Create( m_device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// imgui
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.mSwapchain.mSurfaceFormat.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassImgui.Create( m_device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1);
		}

		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateFramebuffers( const VkExtent2D _extent )
	{	
		const VkFormat depthFormat = m_device.FindDepthFormat();
		const VkFormat colorFormat = m_window.mSwapchain.mSurfaceFormat.format;
		const uint32_t imagesCount = m_window.mSwapchain.mImagesCount;

		// game color
		m_gameColorImage.Create( m_device, colorFormat, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_gameColorImageView.Create( m_device, m_gameColorImage.mImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		// game depth
		m_gameDepthImage.Create( m_device, depthFormat, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		m_gameDepthImageView.Create( m_device, m_gameDepthImage.mImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D );
		{
			VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
			m_gameDepthImage.TransitionImageLayout( cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1 );
			m_device.EndSingleTimeCommands( cmd );
		}
		VkImageView gameViews[2] = { m_gameColorImageView.mImageView, m_gameDepthImageView.mImageView };
		m_gameFrameBuffers.Create( m_device, imagesCount, _extent, m_renderPassGame, gameViews, 2 );

		// pp color		
		m_ppColorImage.Create( m_device, colorFormat, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_ppColorImageView.Create( m_device, m_ppColorImage.mImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		m_ppFramebuffers.Create( m_device, imagesCount, _extent, m_renderPassPostprocess, &m_ppColorImageView.mImageView, 1 );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateTextureDescriptor()
	{		
		const std::vector< Texture* >& textures = Texture::s_resourceManager.GetList();
		std::vector< VkImageView > imageViews( textures.size() );
		for ( int i = 0; i < textures.size(); i++ )
		{
			imageViews[i] = textures[ i ]->mImageView;
		}

		m_imagesDescriptor.Destroy( m_device );
		m_imagesDescriptor.Create( m_device, imageViews.data(), static_cast<uint32_t>( imageViews.size() ) );

		return true;
	}
}