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
#include "render/pipelines/fanPostprocessPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanDebugPipeline.hpp"
#include "render/pipelines/fanImguiPipeline.hpp"
#include "render/pipelines/fanUIPipeline.hpp"
#include "render/fanVertex.hpp"
#include "render/fanWindow.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer( Window& _window ) : m_window( _window )
	{
		Device& device = m_window.GetDevice();
		const uint32_t imagesCount = m_window.GetSwapChain().mImagesCount;

		m_debugLinesvertexBuffers.resize( imagesCount );
		m_debugLinesNoDepthTestVertexBuffers.resize( imagesCount );
		m_debugTrianglesvertexBuffers.resize( imagesCount );

		m_clearColor = glm::vec4( 0.f, 0.f, 0.2f, 1.f );

		Mesh::s_resourceManager.Init(    device );
		Texture::s_resourceManager.Init( device );
		UIMesh::s_resourceManager.Init(  device );

		CreateRenderPasses();
		CreateQuadVertexBuffer();

		m_gameColorSampler.Create( device, 0, 1.f, VK_FILTER_LINEAR );
		m_ppColorSampler.Create( device, 0, 1.f, VK_FILTER_LINEAR );
		CreateFramebuffers( m_window.GetSwapChain().mExtent );
		m_swapchainFramebuffers.CreateForSwapchain( m_window.GetDevice(), imagesCount, m_window.GetSwapChain().mExtent, m_renderPassImgui, m_window.GetSwapChain().mImageViews );

		m_samplerTextures.Create( device, 0, 8, VK_FILTER_LINEAR );
		m_samplerDescriptorTextures.Create( device, m_samplerTextures.mSampler );
		CreateTextureDescriptor();

		CreateShaders();

		m_debugLinesPipeline = new DebugPipeline( device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true );
		m_debugLinesPipeline->CreateDescriptors( device, imagesCount );
		

		m_debugLinesPipelineNoDepthTest = new DebugPipeline( device, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false );
		m_debugLinesPipelineNoDepthTest->CreateDescriptors( device, imagesCount );


		m_debugTrianglesPipeline = new DebugPipeline( device, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false );
		m_debugTrianglesPipeline->CreateDescriptors( device, imagesCount );

		mForwardUniforms.Create( device );
		mForwardDescriptor.AddUniformBinding( device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		mForwardDescriptor.AddDynamicUniformBinding( device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, mForwardUniforms.m_dynamicUniformsVert.Size(), mForwardUniforms.m_dynamicUniformsVert.Alignment() );
		mForwardDescriptor.AddUniformBinding( device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		mForwardDescriptor.AddDynamicUniformBinding( device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, mForwardUniforms.m_dynamicUniformsMaterial.Size(), mForwardUniforms.m_dynamicUniformsMaterial.Alignment() );
		mForwardDescriptor.AddUniformBinding( device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		mForwardDescriptor.Create( device, imagesCount );		

		m_samplerUI.Create( device, 0, 1, VK_FILTER_NEAREST );
		m_samplerDescriptorUI.Create( device, m_samplerUI.mSampler );
		m_uiPipeline = new UIPipeline( device, &m_imagesDescriptor, &m_samplerDescriptorUI );
		m_uiPipeline->CreateDescriptors( device, imagesCount );

		m_postprocessPipeline = new PostprocessPipeline( device );
		m_postprocessPipeline->SetGameImageView( m_gameColorImageView );
		m_postprocessPipeline->CreateDescriptors( device, imagesCount );

		CreatePipelines();

		m_imguiPipeline = new ImguiPipeline( device, imagesCount );
		m_imguiPipeline->SetGameView( m_ppColorImageView );
		m_imguiPipeline->Create( m_renderPassImgui.mRenderPass, m_window.GetWindow(), m_window.GetSwapChain().mExtent );

		CreateCommandBuffers();
		RecordAllCommandBuffers();

		const size_t initialSize = 256;
		m_meshDrawArray.reserve( initialSize );
		mForwardUniforms.m_dynamicUniformsVert.Resize( initialSize );
		mForwardUniforms.m_dynamicUniformsMaterial.Resize( initialSize );
	}

	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer()
	{
		Device& device = m_window.GetDevice();

		vkDeviceWaitIdle( device.mDevice );

		delete m_imguiPipeline;
		m_forwardPipeline.DeletePipeline( device );
		mForwardDescriptor.Destroy( device );
		delete m_uiPipeline;

		Mesh::s_resourceManager.Clear();
		Texture::s_resourceManager.Clear();
		UIMesh::s_resourceManager.Clear();

		m_samplerDescriptorTextures.Destroy( device );
		m_samplerDescriptorUI.Destroy( device );
		m_imagesDescriptor.Destroy( device );

		m_samplerTextures.Destroy( device );
		m_samplerUI.Destroy( device );
		m_quadVertexBuffer.Destroy( device );

		delete m_debugLinesPipeline;
		delete m_debugLinesPipelineNoDepthTest;
		delete m_debugTrianglesPipeline;

		for( int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++ )
		{
			m_debugLinesvertexBuffers[bufferIndex].Destroy( device );
		} m_debugLinesvertexBuffers.clear();

		for( int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++ )
		{
			m_debugLinesNoDepthTestVertexBuffers[bufferIndex].Destroy( device );;
		} m_debugLinesNoDepthTestVertexBuffers.clear();

		for( int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++ )
		{
			m_debugTrianglesvertexBuffers[bufferIndex].Destroy( device );;
		} m_debugTrianglesvertexBuffers.clear();


		// game framebuffers & attachements
		m_gameFrameBuffers.Destroy( device );
		m_gameDepthImage.Destroy( device );
		m_gameDepthImageView.Destroy( device );
		m_gameColorSampler.Destroy( device );
		m_gameColorImage.Destroy( device );
		m_gameColorImageView.Destroy( device );

		// pp frame buffers & attachements
		m_ppFramebuffers.Destroy( device );
		m_ppColorSampler.Destroy( device );
		m_ppColorImage.Destroy( device );
		m_ppColorImageView.Destroy( device );
		
		// render passes
		m_renderPassGame.Destroy( device );
		m_renderPassPostprocess.Destroy( device );
		m_renderPassImgui.Destroy( device );

		m_swapchainFramebuffers.Destroy( device );

		delete m_postprocessPipeline;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreateShaders()
	{
		Device& device = m_window.GetDevice();

		m_debugLinesVertShader.Create( device, "code/shaders/debugLines.vert" );
		m_debugLinesFragShader.Create( device, "code/shaders/debugLines.frag" );
		m_debugLinesNoDepthTestVertShader.Create( device, "code/shaders/debugLines.vert" );
		m_debugLinesNoDepthTestFragShader.Create( device, "code/shaders/debugLines.frag" );
		m_debugTrianglesVertShader.Create( device, "code/shaders/debugTriangles.vert" );
		m_debugTrianglesFragShader.Create( device, "code/shaders/debugTriangles.frag" );
		m_forwardVertShader.Create( device, "code/shaders/forward.vert" );
		m_forwardFragShader.Create( device, "code/shaders/forward.frag" );
		m_uiVertShader.Create( device, "code/shaders/ui.vert" );
		m_uiFragShader.Create( device, "code/shaders/ui.frag" );
		m_ppVertShader.Create( device, "code/shaders/postprocess.vert" );
		m_ppFragShader.Create( device, "code/shaders/postprocess.frag" );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyShaders()
	{
		Device& device = m_window.GetDevice();

		m_debugLinesVertShader				.Destroy( device );
		m_debugLinesFragShader				.Destroy( device );
		m_debugLinesNoDepthTestVertShader	.Destroy( device );
		m_debugLinesNoDepthTestFragShader	.Destroy( device );
		m_debugTrianglesVertShader			.Destroy( device );
		m_debugTrianglesFragShader			.Destroy( device );
		m_forwardVertShader					.Destroy( device );
		m_forwardFragShader					.Destroy( device );
		m_uiVertShader						.Destroy( device );
		m_uiFragShader						.Destroy( device );
		m_ppVertShader						.Destroy( device );
		m_ppFragShader						.Destroy( device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreatePipelines()
	{
		Device& device = m_window.GetDevice();

		m_debugLinesPipeline->CreatePipeline( device, m_debugLinesPipeline->GetConfig( m_debugLinesVertShader, m_debugLinesFragShader ), m_gameExtent, m_renderPassGame.mRenderPass );
		m_debugLinesPipelineNoDepthTest->CreatePipeline( device, m_debugLinesPipelineNoDepthTest->GetConfig( m_debugLinesNoDepthTestVertShader, m_debugLinesNoDepthTestFragShader ), m_gameExtent, m_renderPassGame.mRenderPass );
		m_debugTrianglesPipeline->CreatePipeline( device, m_debugTrianglesPipeline->GetConfig( m_debugTrianglesVertShader, m_debugTrianglesFragShader ), m_gameExtent, m_renderPassGame.mRenderPass );
		m_forwardPipeline.CreatePipeline( device, GetForwardPipelineConfig( m_forwardVertShader, m_forwardFragShader ), m_gameExtent, m_renderPassGame.mRenderPass );
		m_uiPipeline->CreatePipeline( device, m_uiPipeline->GetConfig( m_uiVertShader, m_uiFragShader ), m_gameExtent, m_renderPassPostprocess.mRenderPass );
		m_postprocessPipeline->CreatePipeline( device, m_postprocessPipeline->GetConfig( m_ppVertShader, m_ppFragShader ), m_gameExtent, m_renderPassPostprocess.mRenderPass );
	}
	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyPipelines()
	{
		Device& device = m_window.GetDevice();

		m_postprocessPipeline->DeletePipeline( device );
		m_forwardPipeline.DeletePipeline( device );
		m_uiPipeline->DeletePipeline( device );
		m_debugLinesPipeline->DeletePipeline( device );
		m_debugLinesPipelineNoDepthTest->DeletePipeline( device );
		m_debugTrianglesPipeline->DeletePipeline( device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame()
	{
		SCOPED_PROFILE( draw_frame );

		const VkResult result = m_window.GetSwapChain().AcquireNextImage( m_window.GetDevice() );
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
			vkWaitForFences( m_window.GetDevice().mDevice, 1, m_window.GetSwapChain().GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max() );
			vkResetFences( m_window.GetDevice().mDevice, 1, m_window.GetSwapChain().GetCurrentInFlightFence() );
		}

		ImGui::GetIO().DisplaySize = ImVec2( static_cast< float >( m_window.GetSwapChain().mExtent.width ), static_cast< float >( m_window.GetSwapChain().mExtent.height ) );

		if ( Texture::s_resourceManager.IsModified() )
		{
			WaitIdle();
			Debug::Log( "reload textures" );
			CreateTextureDescriptor();
			Texture::s_resourceManager.SetUnmodified();
		}

		const uint32_t currentFrame = m_window.GetSwapChain().mCurrentFrame;
		UpdateUniformBuffers( m_window.GetDevice(), currentFrame );
		{
			SCOPED_PROFILE( record_cmd );
			RecordCommandBufferGeometry( currentFrame );
			RecordCommandBufferDebug( currentFrame );
			RecordCommandBufferUI( currentFrame );
			RecordCommandBufferImgui( currentFrame );
			RecordPrimaryCommandBuffer( currentFrame );
		}

		{
			SCOPED_PROFILE( submit );
			SubmitCommandBuffers();
			m_window.GetSwapChain().PresentImage( m_window.GetDevice() );
			m_window.GetSwapChain().StartNextFrame();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeGame( btVector2 _newSize )
	{
		Device& device = m_window.GetDevice();

		WaitIdle();
		const VkExtent2D extent = { ( uint32_t ) _newSize[ 0 ], ( uint32_t ) _newSize[ 1 ] };
		m_gameExtent = extent;

		// game framebuffers & attachements
		m_gameDepthImage.Destroy( device );
		m_gameDepthImageView.Destroy( device );
		m_gameColorImage.Destroy( device );
		m_gameColorImageView.Destroy( device );
		m_gameFrameBuffers.Destroy( device );
		// postprocess framebuffers & attachements
		m_ppColorImage.Destroy( device );
		m_ppColorImageView.Destroy( device );
		m_ppFramebuffers.Destroy( device );
		CreateFramebuffers( extent );

		m_postprocessPipeline->m_descriptorImageSampler.Destroy( device );
		m_postprocessPipeline->m_descriptorImageSampler.Create( device, &m_postprocessPipeline->m_imageView->mImageView, 1, m_postprocessPipeline->m_sampler.mSampler );

		m_imguiPipeline->UpdateGameImageDescriptor();

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ResizeSwapchain()
	{
		WaitIdle();
		const VkExtent2D extent = m_window.GetExtent();
		Debug::Get() << Debug::Severity::highlight << "Resize renderer: " << extent.width << "x" << extent.height << Debug::Endl();
		m_window.GetSwapChain().Resize( m_window.GetDevice(), extent );

		m_swapchainFramebuffers.Destroy( m_window.GetDevice() );
		m_swapchainFramebuffers.CreateForSwapchain( m_window.GetDevice(), m_window.GetSwapChain().mImagesCount, extent, m_renderPassImgui, m_window.GetSwapChain().mImageViews );

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		m_postprocessPipeline->SetUniformsData( _device, _index );

		// forward uniforms
		{
			mForwardDescriptor.SetData( _device, 0, _index, &mForwardUniforms.m_vertUniforms, sizeof( VertUniforms ), 0 );
			mForwardDescriptor.SetData( _device, 1, _index, &mForwardUniforms.m_dynamicUniformsVert[0], mForwardUniforms.m_dynamicUniformsVert.Alignment() * mForwardUniforms.m_dynamicUniformsVert.Size(), 0 );
			mForwardDescriptor.SetData( _device, 2, _index, &mForwardUniforms.m_fragUniforms, sizeof( FragUniforms ), 0 );
			mForwardDescriptor.SetData( _device, 3, _index, &mForwardUniforms.m_dynamicUniformsMaterial[0], mForwardUniforms.m_dynamicUniformsMaterial.Alignment() * mForwardUniforms.m_dynamicUniformsMaterial.Size(), 0 );
			mForwardDescriptor.SetData( _device, 4, _index, &mForwardUniforms.m_lightUniforms, sizeof( LightsUniforms ), 0 );
		}
		m_debugLinesPipeline->SetUniformsData( _device, _index );
		m_debugLinesPipelineNoDepthTest->SetUniformsData( _device, _index );
		m_debugTrianglesPipeline->SetUniformsData( _device, _index );
		m_uiPipeline->SetUniformsData( _device, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::WaitIdle()
	{
		vkDeviceWaitIdle( m_window.GetDevice().mDevice );
		Debug::Log( "Renderer idle" );
	}

	//================================================================================================================================
	//================================================================================================================================
	float  Renderer::GetWindowAspectRatio() const
	{
		return static_cast< float >( m_window.GetSwapChain().mExtent.width ) / m_window.GetSwapChain().mExtent.height;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position )
	{
		mForwardUniforms.m_vertUniforms.view = _view;
		mForwardUniforms.m_vertUniforms.proj = _projection;
		mForwardUniforms.m_vertUniforms.proj[1][1] *= -1;

		mForwardUniforms.m_fragUniforms.cameraPosition = _position;

		std::array< DebugPipeline*, 3 > debugLinesPipelines = { m_debugLinesPipeline, m_debugLinesPipelineNoDepthTest, m_debugTrianglesPipeline };
		for( int pipelingIndex = 0; pipelingIndex < debugLinesPipelines.size(); pipelingIndex++ )
		{
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.model = glm::mat4( 1.0 );
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.view = _view;
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.proj = mForwardUniforms.m_vertUniforms.proj;
			debugLinesPipelines[pipelingIndex]->m_debugUniforms.color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetDirectionalLights( const std::vector<DrawDirectionalLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumDirectionalLight );
		mForwardUniforms.m_lightUniforms.dirLightsNum = (uint32_t)_lightData.size();
		for( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawDirectionalLight& light = _lightData[i];
			mForwardUniforms.m_lightUniforms.dirLights[i].direction = light.direction;
			mForwardUniforms.m_lightUniforms.dirLights[i].ambiant = light.ambiant;
			mForwardUniforms.m_lightUniforms.dirLights[i].diffuse = light.diffuse;
			mForwardUniforms.m_lightUniforms.dirLights[i].specular = light.specular;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetPointLights( const std::vector<DrawPointLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumPointLights );
		mForwardUniforms.m_lightUniforms.pointLightNum = (uint32_t)_lightData.size();
		for ( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawPointLight& light = _lightData[i];
			mForwardUniforms.m_lightUniforms.pointlights[i].position	= light.position;
			mForwardUniforms.m_lightUniforms.pointlights[i].diffuse	= light.diffuse;
			mForwardUniforms.m_lightUniforms.pointlights[i].specular	= light.specular;
			mForwardUniforms.m_lightUniforms.pointlights[i].ambiant	= light.ambiant;
			mForwardUniforms.m_lightUniforms.pointlights[i].constant	= light.constant;
			mForwardUniforms.m_lightUniforms.pointlights[i].linear	= light.linear;
			mForwardUniforms.m_lightUniforms.pointlights[i].quadratic = light.quadratic;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDrawData( const std::vector<DrawMesh>& _drawData )
	{
		Device& device = m_window.GetDevice();

		if ( _drawData.size() > m_meshDrawArray.capacity() )
		{
			Debug::Warning( "Resizing draw data arrays" );
			WaitIdle();
			const size_t newSize = 2 * _drawData.size();
			const uint32_t imagesCount = m_window.GetSwapChain().mImagesCount;
			m_meshDrawArray.reserve( newSize );
			mForwardUniforms.m_dynamicUniformsVert.Resize( newSize );
			mForwardUniforms.m_dynamicUniformsMaterial.Resize( newSize );
			mForwardDescriptor.ResizeDynamicUniformBinding( device, imagesCount, mForwardUniforms.m_dynamicUniformsVert.Size(), mForwardUniforms.m_dynamicUniformsVert.Alignment(), 1 );
			mForwardDescriptor.ResizeDynamicUniformBinding( device, imagesCount, mForwardUniforms.m_dynamicUniformsMaterial.Size(), mForwardUniforms.m_dynamicUniformsMaterial.Alignment(), 3 );
			mForwardDescriptor.UpdateDescriptorSets( device, imagesCount );
		}

		m_meshDrawArray.clear();
		for ( int dataIndex = 0; dataIndex < _drawData.size(); dataIndex++ )
		{
			const DrawMesh& data = _drawData[ dataIndex ];

			if ( data.mesh != nullptr && !data.mesh->GetIndices().empty() )
			{
				// Transform
				mForwardUniforms.m_dynamicUniformsVert[dataIndex].modelMat = data.modelMatrix;
				mForwardUniforms.m_dynamicUniformsVert[dataIndex].normalMat = data.normalMatrix;

				// material
				mForwardUniforms.m_dynamicUniformsMaterial[ dataIndex ].color = data.color;
				mForwardUniforms.m_dynamicUniformsMaterial[ dataIndex ].shininess = data.shininess;

				// Mesh
				m_meshDrawArray.push_back( { data.mesh, data.textureIndex } );
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
			m_uiPipeline->m_dynamicUniformsVert[ meshIndex ].position = uiData.position;
			m_uiPipeline->m_dynamicUniformsVert[ meshIndex ].scale = uiData.scale;
			m_uiPipeline->m_dynamicUniformsVert[ meshIndex ].color = uiData.color;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		Device& device = m_window.GetDevice();

		m_hasNoDebugToDraw = _debugLinesNoDepthTest.empty() && _debugLines.empty() && _debugTriangles.empty();
		m_numDebugLines = (int)_debugLines.size();
		m_numDebugLinesNoDepthTest = (int)_debugLinesNoDepthTest.size();
		m_numDebugTriangle = (int)_debugTriangles.size();

		const uint32_t currentFrame = m_window.GetSwapChain().mCurrentFrame;

		SCOPED_PROFILE( update_buffer );
		if( _debugLines.size() > 0 )
		{
			SCOPED_PROFILE( lines );
			m_debugLinesvertexBuffers[currentFrame].Destroy( device );	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			m_debugLinesvertexBuffers[currentFrame].Create( 
				device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( device, _debugLines.data(), size );
				VkCommandBuffer cmd = device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesvertexBuffers[currentFrame].mBuffer, size );
				device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( device );
			}
		}

		if( _debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth );
			m_debugLinesNoDepthTestVertexBuffers[currentFrame].Destroy( device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			m_debugLinesNoDepthTestVertexBuffers[currentFrame].Create(
				device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( device, _debugLinesNoDepthTest.data(), size );
				VkCommandBuffer cmd = device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesNoDepthTestVertexBuffers[currentFrame].mBuffer, size );
				device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( device );
			}
		}

		if( _debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles );
			m_debugTrianglesvertexBuffers[currentFrame].Destroy( device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			m_debugTrianglesvertexBuffers[currentFrame].Create(
				device,
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			if( size > 0 )
			{
				Buffer stagingBuffer;
				stagingBuffer.Create(
					device,
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData( device, _debugTriangles.data(), size );
				VkCommandBuffer cmd = device.BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo( cmd, m_debugTrianglesvertexBuffers[currentFrame].mBuffer, size );
				device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( device );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordAllCommandBuffers()
	{
		for( size_t i = 0; i < m_window.GetSwapChain().mImagesCount; i++ )
		{
			RecordCommandBufferImgui( i );
			RecordCommandBufferGeometry( i );
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
		clearValues[ 0 ].color = { m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a };
		clearValues[ 1 ].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo =			  RenderPass::GetBeginInfo(	m_renderPassGame.mRenderPass,		m_gameFrameBuffers.mFrameBuffers[_index],		  m_gameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoPostprocess = RenderPass::GetBeginInfo( m_renderPassPostprocess.mRenderPass,m_ppFramebuffers.mFrameBuffers[_index],  m_gameExtent, clearValues.data(), (uint32_t)clearValues.size() );
		VkRenderPassBeginInfo renderPassInfoImGui =		  RenderPass::GetBeginInfo( m_renderPassImgui.mRenderPass,		m_swapchainFramebuffers.mFrameBuffers[_index],	  m_window.GetSwapChain().mExtent, clearValues.data(), (uint32_t)clearValues.size() );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_geometryCommandBuffers.mBuffers[ _index ] );
				if ( ! m_hasNoDebugToDraw )
				{
					vkCmdExecuteCommands( commandBuffer, 1, &m_debugCommandBuffers.mBuffers[ _index ] );
				}
			} vkCmdEndRenderPass( commandBuffer );

			vkCmdBeginRenderPass( commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			{
				vkCmdExecuteCommands( commandBuffer, 1, &m_postprocessCommandBuffers.mBuffers[ _index ] );
				vkCmdExecuteCommands( commandBuffer, 1, &m_uiCommandBuffers.mBuffers[ _index ] );
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

		VkCommandBuffer commandBuffer = m_postprocessCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassPostprocess.mRenderPass, m_ppFramebuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_postprocessPipeline->Bind( commandBuffer, m_gameExtent, _index );
			VkBuffer vertexBuffers[] = { m_quadVertexBuffer.mBuffer };
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
		VkCommandBuffer commandBuffer = m_uiCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassPostprocess.mRenderPass, m_ppFramebuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_uiPipeline->Bind( commandBuffer, m_gameExtent, _index );
			m_uiPipeline->BindDescriptors( commandBuffer, _index, 0 );

			VkDeviceSize offsets[] = { 0 };

			for ( uint32_t meshIndex = 0; meshIndex < m_uiMeshDrawArray.size(); meshIndex++ )
			{
				UIDrawData drawData = m_uiMeshDrawArray[ meshIndex ];
				UIMesh* mesh = drawData.mesh;
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer().mBuffer };
				m_uiPipeline->BindDescriptors( commandBuffer, _index, meshIndex );
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				BindTexture( commandBuffer, drawData.textureIndex, &m_samplerDescriptorUI, m_uiPipeline->m_pipelineLayout );
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
		m_imguiPipeline->UpdateBuffer( _index );

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassImgui.mRenderPass, m_swapchainFramebuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_imguiPipeline->DrawFrame( commandBuffer, _index );

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
	void Renderer::RecordCommandBufferGeometry( const size_t _index )
	{
		SCOPED_PROFILE( geometry );
		VkCommandBuffer commandBuffer = m_geometryCommandBuffers.mBuffers[ _index ];
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassGame.mRenderPass, m_gameFrameBuffers.mFrameBuffers[_index] );
		VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

		if ( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
		{
			m_forwardPipeline.Bind( commandBuffer, m_gameExtent, _index );

			for ( uint32_t meshIndex = 0; meshIndex < m_meshDrawArray.size(); meshIndex++ )
			{
				DrawData& drawData = m_meshDrawArray[ meshIndex ];
				BindTexture( commandBuffer, drawData.textureIndex, &m_samplerDescriptorTextures, m_forwardPipeline.m_pipelineLayout );
				BindForwardDescriptors( commandBuffer, _index, meshIndex );
				VkDeviceSize offsets[] = { 0 };
				VkBuffer vertexBuffers[] = { drawData.mesh->GetVertexBuffer().mBuffer };
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				vkCmdBindIndexBuffer( commandBuffer, drawData.mesh->GetIndexBuffer().mBuffer, 0, VK_INDEX_TYPE_UINT32 );
				vkCmdDrawIndexed( commandBuffer, static_cast< uint32_t >( drawData.mesh->GetIndices().size() ), 1, 0, 0, 0 );
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
	void Renderer::RecordCommandBufferDebug( const size_t _index )
	{
		SCOPED_PROFILE( debug );
		if( ! m_hasNoDebugToDraw )
		{
			VkCommandBuffer commandBuffer = m_debugCommandBuffers.mBuffers[_index];
			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo( m_renderPassGame.mRenderPass, m_gameFrameBuffers.mFrameBuffers[_index] );
			VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

			if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
			{
				VkDeviceSize offsets[] = { 0 };
				if( m_numDebugLines > 0 )
				{
					m_debugLinesPipeline->Bind( commandBuffer, m_gameExtent, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesvertexBuffers[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLines ), 1, 0, 0 );
				}
				if( m_numDebugLinesNoDepthTest > 0 )
				{
					m_debugLinesPipelineNoDepthTest->Bind( commandBuffer, m_gameExtent, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesNoDepthTestVertexBuffers[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLinesNoDepthTest ), 1, 0, 0 );
				}
				if( m_numDebugTriangle > 0 )
				{
					m_debugTrianglesPipeline->Bind( commandBuffer, m_gameExtent, _index );
					VkBuffer vertexBuffers[] = { m_debugTrianglesvertexBuffers[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugTriangle ), 1, 0, 0 );
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
			m_primaryCommandBuffers.mBuffers[ m_window.GetSwapChain().mCurrentImageIndex ]
			//, m_imguiCommandBuffers[m_window.GetSwapChain().GetCurrentImageIndex()]
		};

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = m_window.GetSwapChain().GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount = static_cast< uint32_t >( commandBuffers.size() );
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = m_window.GetSwapChain().GetCurrentRenderFinishedSemaphore();

		VkResult result = vkQueueSubmit( m_window.GetDevice().mGraphicsQueue, 1, &submitInfo, *m_window.GetSwapChain().GetCurrentInFlightFence() );
		if ( result != VK_SUCCESS )
		{
			Debug::Error( "Could not submit draw command buffer " );
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders()
	{
		Debug::Highlight( "Reloading shaders" );
		Device& device = m_window.GetDevice();

		vkDeviceWaitIdle( device.mDevice );
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
		m_imguiPipeline->ReloadIcons();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateCommandBuffers()
	{
		const uint32_t count = m_window.GetSwapChain().mImagesCount;

		bool result = true;

		result &= m_primaryCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		result &= m_geometryCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_imguiCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_uiCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_postprocessCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_debugCommandBuffers.Create( m_window.GetDevice(), count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );

		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPasses()
	{
		Device& device = m_window.GetDevice();
		bool result = true;

		// game
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkAttachmentDescription depthAtt = RenderPass::GetDepthAttachment( device.FindDepthFormat() );
			VkAttachmentReference	depthAttRef = RenderPass::GetDepthAttachmentReference( 1 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, &depthAttRef );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			
			VkAttachmentDescription attachmentDescriptions[2] = { colorAtt, depthAtt };
			result &= m_renderPassGame.Create( device, attachmentDescriptions, 2, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// postprocess
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassPostprocess.Create( device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// imgui
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().mSurfaceFormat.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassImgui.Create( device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1);
		}

		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateFramebuffers( const VkExtent2D _extent )
	{
		Device& device = m_window.GetDevice();		
		const VkFormat depthFormat = device.FindDepthFormat();
		const VkFormat colorFormat = m_window.GetSwapChain().mSurfaceFormat.format;
		const uint32_t imagesCount = m_window.GetSwapChain().mImagesCount;

		// game color
		m_gameColorImage.Create( device, colorFormat, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_gameColorImageView.Create( device, m_gameColorImage.mImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		// game depth
		m_gameDepthImage.Create( device, depthFormat, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		m_gameDepthImageView.Create( device, m_gameDepthImage.mImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D );
		{
			VkCommandBuffer cmd = device.BeginSingleTimeCommands();
			m_gameDepthImage.TransitionImageLayout( cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1 );
			device.EndSingleTimeCommands( cmd );
		}
		VkImageView gameViews[2] = { m_gameColorImageView.mImageView, m_gameDepthImageView.mImageView };
		m_gameFrameBuffers.Create( device, imagesCount, _extent, m_renderPassGame, gameViews, 2 );

		// pp color		
		m_ppColorImage.Create( device, colorFormat, _extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_ppColorImageView.Create( device, m_ppColorImage.mImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		m_ppFramebuffers.Create( device, imagesCount, _extent, m_renderPassPostprocess, &m_ppColorImageView.mImageView, 1 );
	}

	//================================================================================================================================
	// Used for postprocess
	//================================================================================================================================
	void Renderer::CreateQuadVertexBuffer()
	{
		Device& device = m_window.GetDevice();

		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof( vertices[ 0 ] ) * vertices.size();
		m_quadVertexBuffer.Create(
			device,
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		Buffer stagingBuffer;
		stagingBuffer.Create(
			device,
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer.SetData( device, vertices.data(), size );
		VkCommandBuffer cmd = device.BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo( cmd, m_quadVertexBuffer.mBuffer, size );
		device.EndSingleTimeCommands( cmd );
		stagingBuffer.Destroy( device );
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

		m_imagesDescriptor.Destroy( m_window.GetDevice() );
		m_imagesDescriptor.Create( m_window.GetDevice(), imageViews.data(), static_cast<uint32_t>( imageViews.size() ) );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler* _samplerDescriptor, VkPipelineLayout _pipelineLayout )
	{
		assert( _textureIndex < Texture::s_resourceManager.GetList().size() );

		std::vector<VkDescriptorSet> descriptors = {
			 m_imagesDescriptor.mDescriptorSets[ _textureIndex ]
			 , _samplerDescriptor->mDescriptorSet
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1,
			static_cast< uint32_t >( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::BindForwardDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			mForwardDescriptor.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast<uint32_t>( mForwardUniforms.m_dynamicUniformsVert.Alignment() )
			,_indexOffset* static_cast<uint32_t>( mForwardUniforms.m_dynamicUniformsMaterial.Alignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_forwardPipeline.m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig Renderer::GetForwardPipelineConfig( Shader& _vert, Shader& _frag )
	{
		PipelineConfig config( _vert, _frag );

		config.bindingDescription = Vertex::GetBindingDescription();
		config.attributeDescriptions = Vertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			mForwardDescriptor.mDescriptorSetLayout
			, m_imagesDescriptor.mDescriptorSetLayout
			, m_samplerDescriptorTextures.mDescriptorSetLayout
		};

		return config;
	}
}