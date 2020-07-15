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
	Renderer::Renderer( Window& _window ) : m_window( _window ), m_device( _window.GetDevice() )
	{
		const uint32_t imagesCount = m_window.GetSwapChain().mImagesCount;

		m_debugLinesvertexBuffers.resize( imagesCount );
		m_debugLinesNoDepthTestVertexBuffers.resize( imagesCount );
		m_debugTrianglesvertexBuffers.resize( imagesCount );

		m_clearColor = glm::vec4( 0.f, 0.f, 0.2f, 1.f );

		Mesh::s_resourceManager.Init( m_device );
		Texture::s_resourceManager.Init( m_device );
		UIMesh::s_resourceManager.Init( m_device );

		CreateRenderPasses();
		CreateQuadVertexBuffer();

		m_gameColorSampler.Create( m_device, 0, 1.f, VK_FILTER_LINEAR );
		m_ppColorSampler.Create( m_device, 0, 1.f, VK_FILTER_LINEAR );
		CreateFramebuffers( m_window.GetSwapChain().mExtent );
		m_swapchainFramebuffers.CreateForSwapchain( m_device, imagesCount, m_window.GetSwapChain().mExtent, m_renderPassImgui, m_window.GetSwapChain().mImageViews );

		m_samplerTextures.Create( m_device, 0, 8, VK_FILTER_LINEAR );
		m_samplerDescriptorTextures.Create( m_device, m_samplerTextures.mSampler );
		CreateTextureDescriptor();

		CreateShaders();

		m_debugLinesDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_debugLinesDescriptor.Create( m_device, imagesCount );

		m_debugLinesNoDepthTestDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_debugLinesNoDepthTestDescriptor.Create( m_device, imagesCount );

		m_debugTrianglesDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_debugTrianglesDescriptor.Create( m_device, imagesCount );

		m_forwardUniforms.Create( m_device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
		m_uiUniforms.Create( m_device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );

		m_forwardDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		m_forwardDescriptor.AddDynamicUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, m_forwardUniforms.m_dynamicUniformsVert.Size(), m_forwardUniforms.m_dynamicUniformsVert.Alignment() );
		m_forwardDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_forwardDescriptor.AddDynamicUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, m_forwardUniforms.m_dynamicUniformsMaterial.Size(), m_forwardUniforms.m_dynamicUniformsMaterial.Alignment() );
		m_forwardDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		m_forwardDescriptor.Create( m_device, imagesCount );		

		m_samplerUI.Create( m_device, 0, 1, VK_FILTER_NEAREST );
		m_samplerDescriptorUI.Create( m_device, m_samplerUI.mSampler );
		
		m_uiTransformDescriptor.AddDynamicUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_VERTEX_BIT, m_uiUniforms.mUidynamicUniformsVert.Size(), m_uiUniforms.mUidynamicUniformsVert.Alignment() );
		m_uiTransformDescriptor.Create( m_device, imagesCount );		

		m_ppSampler.Create( m_device, 0, 0.f, VK_FILTER_NEAREST );
		
		m_ppUniformDescriptor.Destroy( m_device );
		m_ppUniformDescriptor.AddUniformBinding( m_device, imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( PostprocessUniforms ) );
		m_ppUniformDescriptor.Create( m_device, imagesCount );
		m_ppUniformDescriptor.SetData( m_device, 0, 0, &m_ppUniforms, sizeof( PostprocessUniforms ), 0 );

		m_ppDescriptorImage.Create( m_device, &m_gameColorImageView.mImageView, 1, m_ppSampler.mSampler );

		CreatePipelines();

		m_imguiPipeline.Create( m_device, imagesCount, m_renderPassImgui.mRenderPass, m_window.GetWindow(), m_window.GetSwapChain().mExtent, m_ppColorImageView );

		CreateCommandBuffers();
		RecordAllCommandBuffers();

		const size_t initialSize = 256;
		m_meshDrawArray.reserve( initialSize );
		m_forwardUniforms.m_dynamicUniformsVert.Resize( initialSize );
		m_forwardUniforms.m_dynamicUniformsMaterial.Resize( initialSize );
	}

	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer()
	{
		vkDeviceWaitIdle( m_device.mDevice );

		m_imguiPipeline.Destroy( m_device );
		m_forwardPipeline.Destroy( m_device );
		m_forwardDescriptor.Destroy( m_device );
		m_uiPipeline.Destroy( m_device );
		m_uiTransformDescriptor.Destroy( m_device );

		m_debugLinesDescriptor.Destroy( m_device );
		m_debugLinesNoDepthTestDescriptor.Destroy( m_device );
		m_debugTrianglesDescriptor.Destroy( m_device );

		Mesh::s_resourceManager.Clear();
		Texture::s_resourceManager.Clear();
		UIMesh::s_resourceManager.Clear();

		m_samplerDescriptorTextures.Destroy( m_device );
		m_samplerDescriptorUI.Destroy( m_device );
		m_imagesDescriptor.Destroy( m_device );

		m_samplerTextures.Destroy( m_device );
		m_samplerUI.Destroy( m_device );
		m_quadVertexBuffer.Destroy( m_device );

		m_debugLinesPipeline.Destroy( m_device );
		m_debugLinesPipelineNoDepthTest.Destroy( m_device );
		m_debugTrianglesPipeline.Destroy( m_device );

		for( int bufferIndex = 0; bufferIndex < m_debugLinesvertexBuffers.size(); bufferIndex++ )
		{
			m_debugLinesvertexBuffers[bufferIndex].Destroy( m_device );
		} m_debugLinesvertexBuffers.clear();

		for( int bufferIndex = 0; bufferIndex < m_debugLinesNoDepthTestVertexBuffers.size(); bufferIndex++ )
		{
			m_debugLinesNoDepthTestVertexBuffers[bufferIndex].Destroy( m_device );;
		} m_debugLinesNoDepthTestVertexBuffers.clear();

		for( int bufferIndex = 0; bufferIndex < m_debugTrianglesvertexBuffers.size(); bufferIndex++ )
		{
			m_debugTrianglesvertexBuffers[bufferIndex].Destroy( m_device );;
		} m_debugTrianglesvertexBuffers.clear();


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

		m_postprocessPipeline.Destroy( m_device );
		m_ppDescriptorImage.Destroy( m_device );
		m_ppUniformDescriptor.Destroy( m_device );
		m_ppSampler.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreateShaders()
	{
		m_debugLinesVertShader.Create( m_device, "code/shaders/debugLines.vert" );
		m_debugLinesFragShader.Create( m_device, "code/shaders/debugLines.frag" );
		m_debugLinesNoDepthTestVertShader.Create( m_device, "code/shaders/debugLines.vert" );
		m_debugLinesNoDepthTestFragShader.Create( m_device, "code/shaders/debugLines.frag" );
		m_debugTrianglesVertShader.Create( m_device, "code/shaders/debugTriangles.vert" );
		m_debugTrianglesFragShader.Create( m_device, "code/shaders/debugTriangles.frag" );
		m_forwardVertShader.Create( m_device, "code/shaders/forward.vert" );
		m_forwardFragShader.Create( m_device, "code/shaders/forward.frag" );
		m_uiVertShader.Create( m_device, "code/shaders/ui.vert" );
		m_uiFragShader.Create( m_device, "code/shaders/ui.frag" );
		m_ppVertShader.Create( m_device, "code/shaders/postprocess.vert" );
		m_ppFragShader.Create( m_device, "code/shaders/postprocess.frag" );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyShaders()
	{
		m_debugLinesVertShader				.Destroy( m_device );
		m_debugLinesFragShader				.Destroy( m_device );
		m_debugLinesNoDepthTestVertShader	.Destroy( m_device );
		m_debugLinesNoDepthTestFragShader	.Destroy( m_device );
		m_debugTrianglesVertShader			.Destroy( m_device );
		m_debugTrianglesFragShader			.Destroy( m_device );
		m_forwardVertShader					.Destroy( m_device );
		m_forwardFragShader					.Destroy( m_device );
		m_uiVertShader						.Destroy( m_device );
		m_uiFragShader						.Destroy( m_device );
		m_ppVertShader						.Destroy( m_device );
		m_ppFragShader						.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::CreatePipelines()
	{		
		PipelineConfig debugLinesPipelineConfig = GetDebugPipelineConfig( m_debugLinesVertShader, m_debugLinesFragShader, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, true, m_debugLinesDescriptor );
		PipelineConfig debugLinesNoDepthTestPipelineConfig = GetDebugPipelineConfig( m_debugLinesNoDepthTestVertShader, m_debugLinesNoDepthTestFragShader, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false, m_debugLinesNoDepthTestDescriptor );
		PipelineConfig debugTrianglesPipelineConfig = GetDebugPipelineConfig( m_debugTrianglesVertShader, m_debugTrianglesFragShader, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, m_debugTrianglesDescriptor );
		PipelineConfig ppPipelineConfig = GetPostprocessPipelineConfig( m_ppVertShader, m_ppFragShader );

		m_debugLinesPipeline.Create( m_device, debugLinesPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		m_debugLinesPipelineNoDepthTest.Create( m_device, debugLinesNoDepthTestPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		m_debugTrianglesPipeline.Create( m_device, debugTrianglesPipelineConfig, m_gameExtent, m_renderPassGame.mRenderPass );
		m_forwardPipeline.Create( m_device, GetForwardPipelineConfig( m_forwardVertShader, m_forwardFragShader ), m_gameExtent, m_renderPassGame.mRenderPass );
		m_uiPipeline.Create( m_device, GetUiPipelineConfig( m_uiVertShader, m_uiFragShader ), m_gameExtent, m_renderPassPostprocess.mRenderPass );
		m_postprocessPipeline.Create( m_device, ppPipelineConfig, m_gameExtent, m_renderPassPostprocess.mRenderPass );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DestroyPipelines()
	{
		m_postprocessPipeline.Destroy( m_device );
		m_forwardPipeline.Destroy( m_device );
		m_uiPipeline.Destroy( m_device );
		m_debugLinesPipeline.Destroy( m_device );
		m_debugLinesPipelineNoDepthTest.Destroy( m_device );
		m_debugTrianglesPipeline.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame()
	{
		SCOPED_PROFILE( draw_frame );

		const VkResult result = m_window.GetSwapChain().AcquireNextImage( m_device );
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
			vkWaitForFences( m_device.mDevice, 1, m_window.GetSwapChain().GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max() );
			vkResetFences( m_device.mDevice, 1, m_window.GetSwapChain().GetCurrentInFlightFence() );
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
		UpdateUniformBuffers( m_device, currentFrame );
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
			m_window.GetSwapChain().PresentImage( m_device );
			m_window.GetSwapChain().StartNextFrame();
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

		m_ppDescriptorImage.Destroy( m_device );
		m_ppDescriptorImage.Create( m_device, &m_gameColorImageView.mImageView, 1, m_ppSampler.mSampler );

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
		m_window.GetSwapChain().Resize( m_device , extent );

		m_swapchainFramebuffers.Destroy( m_device );
		m_swapchainFramebuffers.CreateForSwapchain( m_device, m_window.GetSwapChain().mImagesCount, extent, m_renderPassImgui, m_window.GetSwapChain().mImageViews );

		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		m_ppUniformDescriptor.SetData( _device, 0, _index, &m_ppUniforms, sizeof( PostprocessUniforms ), 0 );

		// forward uniforms
		{
			m_forwardDescriptor.SetData( _device, 0, _index, &m_forwardUniforms.m_vertUniforms, sizeof( VertUniforms ), 0 );
			m_forwardDescriptor.SetData( _device, 1, _index, &m_forwardUniforms.m_dynamicUniformsVert[0], m_forwardUniforms.m_dynamicUniformsVert.Alignment() * m_forwardUniforms.m_dynamicUniformsVert.Size(), 0 );
			m_forwardDescriptor.SetData( _device, 2, _index, &m_forwardUniforms.m_fragUniforms, sizeof( FragUniforms ), 0 );
			m_forwardDescriptor.SetData( _device, 3, _index, &m_forwardUniforms.m_dynamicUniformsMaterial[0], m_forwardUniforms.m_dynamicUniformsMaterial.Alignment() * m_forwardUniforms.m_dynamicUniformsMaterial.Size(), 0 );
			m_forwardDescriptor.SetData( _device, 4, _index, &m_forwardUniforms.m_lightUniforms, sizeof( LightsUniforms ), 0 );
		}
		m_debugLinesDescriptor.SetData( _device, 0, _index, &m_debugLinesUniforms, sizeof( DebugUniforms ), 0 );
		m_debugLinesNoDepthTestDescriptor.SetData( _device, 0, _index, &m_debugLinesNoDepthTestUniforms, sizeof( DebugUniforms ), 0 );
		m_debugTrianglesDescriptor.SetData( _device, 0, _index, &m_debugTrianglesUniforms, sizeof( DebugUniforms ), 0 );
		
		m_uiTransformDescriptor.SetData( _device, 0, _index, &m_uiUniforms.mUidynamicUniformsVert[0], m_uiUniforms.mUidynamicUniformsVert.Alignment() * m_uiUniforms.mUidynamicUniformsVert.Size(), 0 );
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
		m_forwardUniforms.m_vertUniforms.view = _view;
		m_forwardUniforms.m_vertUniforms.proj = _projection;
		m_forwardUniforms.m_vertUniforms.proj[1][1] *= -1;

		m_forwardUniforms.m_fragUniforms.cameraPosition = _position;

		DebugUniforms * debugUniforms[3] = { &m_debugLinesUniforms, &m_debugLinesNoDepthTestUniforms, &m_debugTrianglesUniforms };
		for( int i = 0; i < 3; i++ )
		{
			debugUniforms[i]->model = glm::mat4( 1.0 );
			debugUniforms[i]->view = _view;
			debugUniforms[i]->proj = m_forwardUniforms.m_vertUniforms.proj;
			debugUniforms[i]->color = glm::vec4( 1, 1, 1, 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Renderer::SetDirectionalLights( const std::vector<DrawDirectionalLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumDirectionalLight );
		m_forwardUniforms.m_lightUniforms.dirLightsNum = (uint32_t)_lightData.size();
		for( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawDirectionalLight& light = _lightData[i];
			m_forwardUniforms.m_lightUniforms.dirLights[i].direction = light.direction;
			m_forwardUniforms.m_lightUniforms.dirLights[i].ambiant = light.ambiant;
			m_forwardUniforms.m_lightUniforms.dirLights[i].diffuse = light.diffuse;
			m_forwardUniforms.m_lightUniforms.dirLights[i].specular = light.specular;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetPointLights( const std::vector<DrawPointLight>& _lightData )
	{
		assert( _lightData.size() < RenderGlobal::s_maximumNumPointLights );
		m_forwardUniforms.m_lightUniforms.pointLightNum = (uint32_t)_lightData.size();
		for ( int i = 0; i < _lightData.size(); ++i )
		{
			const DrawPointLight& light = _lightData[i];
			m_forwardUniforms.m_lightUniforms.pointlights[i].position	= light.position;
			m_forwardUniforms.m_lightUniforms.pointlights[i].diffuse	= light.diffuse;
			m_forwardUniforms.m_lightUniforms.pointlights[i].specular	= light.specular;
			m_forwardUniforms.m_lightUniforms.pointlights[i].ambiant	= light.ambiant;
			m_forwardUniforms.m_lightUniforms.pointlights[i].constant	= light.constant;
			m_forwardUniforms.m_lightUniforms.pointlights[i].linear	= light.linear;
			m_forwardUniforms.m_lightUniforms.pointlights[i].quadratic = light.quadratic;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDrawData( const std::vector<DrawMesh>& _drawData )
	{
		if ( _drawData.size() > m_meshDrawArray.capacity() )
		{
			Debug::Warning( "Resizing draw data arrays" );
			WaitIdle();
			const size_t newSize = 2 * _drawData.size();
			const uint32_t imagesCount = m_window.GetSwapChain().mImagesCount;
			m_meshDrawArray.reserve( newSize );
			m_forwardUniforms.m_dynamicUniformsVert.Resize( newSize );
			m_forwardUniforms.m_dynamicUniformsMaterial.Resize( newSize );
			m_forwardDescriptor.ResizeDynamicUniformBinding( m_device, imagesCount, m_forwardUniforms.m_dynamicUniformsVert.Size(), m_forwardUniforms.m_dynamicUniformsVert.Alignment(), 1 );
			m_forwardDescriptor.ResizeDynamicUniformBinding( m_device, imagesCount, m_forwardUniforms.m_dynamicUniformsMaterial.Size(), m_forwardUniforms.m_dynamicUniformsMaterial.Alignment(), 3 );
			m_forwardDescriptor.UpdateDescriptorSets( m_device, imagesCount );
		}

		m_meshDrawArray.clear();
		for ( int dataIndex = 0; dataIndex < _drawData.size(); dataIndex++ )
		{
			const DrawMesh& data = _drawData[ dataIndex ];

			if ( data.mesh != nullptr && !data.mesh->GetIndices().empty() )
			{
				// Transform
				m_forwardUniforms.m_dynamicUniformsVert[dataIndex].modelMat = data.modelMatrix;
				m_forwardUniforms.m_dynamicUniformsVert[dataIndex].normalMat = data.normalMatrix;

				// material
				m_forwardUniforms.m_dynamicUniformsMaterial[ dataIndex ].color = data.color;
				m_forwardUniforms.m_dynamicUniformsMaterial[ dataIndex ].shininess = data.shininess;

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
			m_uiUniforms.mUidynamicUniformsVert[meshIndex].position = uiData.position;
			m_uiUniforms.mUidynamicUniformsVert[meshIndex].scale = uiData.scale;
			m_uiUniforms.mUidynamicUniformsVert[meshIndex].color = uiData.color;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::SetDebugDrawData( const std::vector<DebugVertex>& _debugLines, const std::vector<DebugVertex>& _debugLinesNoDepthTest, const std::vector<DebugVertex>& _debugTriangles )
	{
		m_hasNoDebugToDraw = _debugLinesNoDepthTest.empty() && _debugLines.empty() && _debugTriangles.empty();
		m_numDebugLines = (int)_debugLines.size();
		m_numDebugLinesNoDepthTest = (int)_debugLinesNoDepthTest.size();
		m_numDebugTriangle = (int)_debugTriangles.size();

		const uint32_t currentFrame = m_window.GetSwapChain().mCurrentFrame;

		SCOPED_PROFILE( update_buffer );
		if( _debugLines.size() > 0 )
		{
			SCOPED_PROFILE( lines );
			m_debugLinesvertexBuffers[currentFrame].Destroy( m_device );	// TODO update instead of delete
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLines.size();
			m_debugLinesvertexBuffers[currentFrame].Create( 
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
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesvertexBuffers[currentFrame].mBuffer, size );
				m_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( m_device );
			}
		}

		if( _debugLinesNoDepthTest.size() > 0 )
		{
			SCOPED_PROFILE( lines_no_depth );
			m_debugLinesNoDepthTestVertexBuffers[currentFrame].Destroy( m_device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugLinesNoDepthTest.size();
			m_debugLinesNoDepthTestVertexBuffers[currentFrame].Create(
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
				stagingBuffer.CopyBufferTo( cmd, m_debugLinesNoDepthTestVertexBuffers[currentFrame].mBuffer, size );
				m_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( m_device );
			}
		}

		if( _debugTriangles.size() > 0 )
		{
			SCOPED_PROFILE( triangles );
			m_debugTrianglesvertexBuffers[currentFrame].Destroy( m_device );
			const VkDeviceSize size = sizeof( DebugVertex ) * _debugTriangles.size();
			m_debugTrianglesvertexBuffers[currentFrame].Create(
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
				stagingBuffer.CopyBufferTo( cmd, m_debugTrianglesvertexBuffers[currentFrame].mBuffer, size );
				m_device.EndSingleTimeCommands( cmd );
				stagingBuffer.Destroy( m_device );
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
			m_postprocessPipeline.Bind( commandBuffer, m_gameExtent, _index );
			BindPostprocessDescriptors( commandBuffer, _index );
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
			m_uiPipeline.Bind( commandBuffer, m_gameExtent, _index );
			//m_uiPipeline.BindDescriptors( commandBuffer, _index, 0 );

			VkDeviceSize offsets[] = { 0 };

			for ( uint32_t meshIndex = 0; meshIndex < m_uiMeshDrawArray.size(); meshIndex++ )
			{
				UIDrawData drawData = m_uiMeshDrawArray[ meshIndex ];
				UIMesh* mesh = drawData.mesh;
				VkBuffer vertexBuffers[] = { mesh->GetVertexBuffer().mBuffer };
				BindUIDescriptors( commandBuffer, _index, meshIndex );
				vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
				BindTexture( commandBuffer, drawData.textureIndex, &m_samplerDescriptorUI, m_uiPipeline.m_pipelineLayout );
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
					m_debugLinesPipeline.Bind( commandBuffer, m_gameExtent, _index );
					m_debugLinesDescriptor.Bind( commandBuffer, m_debugLinesPipeline.m_pipelineLayout, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesvertexBuffers[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLines ), 1, 0, 0 );
				}
				if( m_numDebugLinesNoDepthTest > 0 )
				{
					m_debugLinesPipelineNoDepthTest.Bind( commandBuffer, m_gameExtent, _index );
					m_debugLinesNoDepthTestDescriptor.Bind( commandBuffer, m_debugLinesPipelineNoDepthTest.m_pipelineLayout, _index );
					VkBuffer vertexBuffers[] = { m_debugLinesNoDepthTestVertexBuffers[_index].mBuffer };
					vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
					vkCmdDraw( commandBuffer, static_cast<uint32_t>( m_numDebugLinesNoDepthTest ), 1, 0, 0 );
				}
				if( m_numDebugTriangle > 0 )
				{
					m_debugTrianglesPipeline.Bind( commandBuffer, m_gameExtent, _index );
					m_debugTrianglesDescriptor.Bind( commandBuffer, m_debugTrianglesPipeline.m_pipelineLayout, _index );
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

		VkResult result = vkQueueSubmit( m_device.mGraphicsQueue, 1, &submitInfo, *m_window.GetSwapChain().GetCurrentInFlightFence() );
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
	bool Renderer::CreateCommandBuffers()
	{
		const uint32_t count = m_window.GetSwapChain().mImagesCount;

		bool result = true;

		result &= m_primaryCommandBuffers.Create(		m_device, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY );
		result &= m_geometryCommandBuffers.Create(		m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_imguiCommandBuffers.Create(			m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_uiCommandBuffers.Create(			m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_postprocessCommandBuffers.Create(	m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
		result &= m_debugCommandBuffers.Create(			m_device, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );

		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPasses()
	{
		bool result = true;

		// game
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
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
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().mSurfaceFormat.format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			VkAttachmentReference	colorAttRef = RenderPass::GetColorAttachmentReference( 0 );
			VkSubpassDescription	subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef, 1, VK_NULL_HANDLE );
			VkSubpassDependency		subpassDependency = RenderPass::GetDependency();
			result &= m_renderPassPostprocess.Create( m_device, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1 );
		}

		// imgui
		{
			VkAttachmentDescription colorAtt = RenderPass::GetColorAttachment( m_window.GetSwapChain().mSurfaceFormat.format, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
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
		const VkFormat colorFormat = m_window.GetSwapChain().mSurfaceFormat.format;
		const uint32_t imagesCount = m_window.GetSwapChain().mImagesCount;

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
	// Used for postprocess
	//================================================================================================================================
	void Renderer::CreateQuadVertexBuffer()
	{
		// Vertex quad
		glm::vec3 v0 = { -1.0f, -1.0f, 0.0f };
		glm::vec3 v1 = { -1.0f, 1.0f, 0.0f };
		glm::vec3 v2 = { 1.0f, -1.0f, 0.0f };
		glm::vec3 v3 = { 1.0f, 1.0f, 0.0f };
		std::vector<glm::vec3> vertices = { v0, v1 ,v2 ,v3 };

		const VkDeviceSize size = sizeof( vertices[ 0 ] ) * vertices.size();
		m_quadVertexBuffer.Create(
			m_device,
			size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		Buffer stagingBuffer;
		stagingBuffer.Create(
			m_device,
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer.SetData( m_device, vertices.data(), size );
		VkCommandBuffer cmd = m_device.BeginSingleTimeCommands();
		stagingBuffer.CopyBufferTo( cmd, m_quadVertexBuffer.mBuffer, size );
		m_device.EndSingleTimeCommands( cmd );
		stagingBuffer.Destroy( m_device );
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
			m_forwardDescriptor.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast<uint32_t>( m_forwardUniforms.m_dynamicUniformsVert.Alignment() )
			,_indexOffset* static_cast<uint32_t>( m_forwardUniforms.m_dynamicUniformsMaterial.Alignment() )
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
	void Renderer::BindUIDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			m_uiTransformDescriptor.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast<uint32_t>( m_uiUniforms.mUidynamicUniformsVert.Alignment() )
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_uiPipeline.m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::BindPostprocessDescriptors( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		std::vector<VkDescriptorSet> descriptors = {
			m_ppDescriptorImage.mDescriptorSets[0]
			, m_ppUniformDescriptor.mDescriptorSets[_index]
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_postprocessPipeline.m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
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
			m_forwardDescriptor.mDescriptorSetLayout
			, m_imagesDescriptor.mDescriptorSetLayout
			, m_samplerDescriptorTextures.mDescriptorSetLayout
		};

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig Renderer::GetUiPipelineConfig( Shader& _vert, Shader& _frag )
	{
		PipelineConfig config( _vert, _frag );

		config.bindingDescription = UIVertex::GetBindingDescription();
		config.attributeDescriptions = UIVertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			 m_uiTransformDescriptor.mDescriptorSetLayout
			,m_imagesDescriptor.mDescriptorSetLayout
			, m_samplerDescriptorUI.mDescriptorSetLayout
		};

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig Renderer::GetDebugPipelineConfig( Shader& _vert, Shader& _frag, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable, DescriptorUniforms& _descriptor )
	{
		PipelineConfig config( _vert, _frag );

		config.bindingDescription = DebugVertex::GetBindingDescription();
		config.attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		config.inputAssemblyStateCreateInfo.topology = _primitiveTopology;
		config.depthStencilStateCreateInfo.depthTestEnable = _depthTestEnable ? VK_TRUE : VK_FALSE;
		config.descriptorSetLayouts = { _descriptor.mDescriptorSetLayout };
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig Renderer::GetPostprocessPipelineConfig( Shader& _vert, Shader& _frag )
	{
		PipelineConfig config( _vert, _frag );

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
		config.descriptorSetLayouts = { m_ppDescriptorImage.mDescriptorSetLayout, m_ppUniformDescriptor.mDescriptorSetLayout };

		return config;
	}
}