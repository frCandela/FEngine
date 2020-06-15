#include "render/pipelines/fanImguiPipeline.hpp"

#include <array>
#include "imgui/imgui.h"
#include "core/fanDebug.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanImageView.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ImguiPipeline::ImguiPipeline( Device& _device, const int _swapchainImagesCount ) :
		m_device( _device )
		, m_fontTexture( new Texture() )
		, m_iconsTexture( new Texture() )
		, m_sampler( new Sampler( _device ) )
		, m_iconsSampler( new Sampler( _device ) )
	{
		m_vertexBuffers.reserve( _swapchainImagesCount );
		m_indexBuffers.reserve( _swapchainImagesCount );
		for ( int imageIndex = 0; imageIndex < _swapchainImagesCount; imageIndex++ )
		{
			m_vertexBuffers.push_back( Buffer( _device ) );
			m_indexBuffers.push_back( Buffer( _device ) );
		}
		m_vertexCount.resize( _swapchainImagesCount, 0 );
		m_indexCount.resize( _swapchainImagesCount, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	ImguiPipeline::~ImguiPipeline()
	{
		delete( m_fontTexture );
		delete ( m_iconsTexture );
		delete( m_sampler );
		delete( m_iconsSampler );
		delete( m_fragShader );
		delete( m_vertShader );

		vkDestroyPipelineCache( m_device.vkDevice, m_pipelineCache, nullptr );
		vkDestroyPipeline( m_device.vkDevice, m_pipeline, nullptr );
		vkDestroyPipelineLayout( m_device.vkDevice, m_pipelineLayout, nullptr );
		vkDestroyDescriptorPool( m_device.vkDevice, m_descriptorPool, nullptr );
		vkDestroyDescriptorSetLayout( m_device.vkDevice, m_descriptorSetLayout, nullptr );

		ImGui::DestroyContext();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::Create( VkRenderPass _renderPass, GLFWwindow* _window, VkExtent2D _extent )
	{

		ImGui::CreateContext();

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		InitImgui( _window, _extent );
		CreateFontAndSampler();
		CreateDescriptors();
		CreateGraphicsPipeline( _renderPass );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::ReloadIcons()
	{
		Debug::Log( "reloading icons" );

		delete ( m_iconsTexture );
		m_iconsTexture = new Texture();
		m_iconsTexture->LoadFromFile( RenderGlobal::s_defaultIcons );

		VkDescriptorImageInfo iconsDescriptorImageInfo{};
		iconsDescriptorImageInfo.sampler = m_iconsSampler->GetSampler();
		iconsDescriptorImageInfo.imageView = m_iconsTexture->GetImageView();
		iconsDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSetIcons{};
		writeDescriptorSetIcons.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSetIcons.dstSet = m_descriptorSets[ 1 ];
		writeDescriptorSetIcons.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSetIcons.dstBinding = 0;
		writeDescriptorSetIcons.pImageInfo = &iconsDescriptorImageInfo;
		writeDescriptorSetIcons.descriptorCount = 1;

		vkUpdateDescriptorSets( m_device.vkDevice, 1, &writeDescriptorSetIcons, 0, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::UpdateBuffer( const size_t _index )
	{
		ImDrawData* imDrawData = ImGui::GetDrawData();

		// Update buffers only if the imgui command list is not empty
		if ( imDrawData && imDrawData->CmdListsCount > 0 )
		{
			// Note: Alignment is done inside buffer creation
			VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof( ImDrawVert );
			VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof( ImDrawIdx );

			// Update buffers only if vertex or index count has been changed compared to current buffer size

			// Vertex buffer
			Buffer& vertexBuffer = m_vertexBuffers[ _index ];
			if ( ( vertexBuffer.GetBuffer() == VK_NULL_HANDLE ) || ( m_vertexCount[ _index ] != imDrawData->TotalVtxCount ) )
			{
				vertexBuffer.Unmap();
				vertexBuffer.Destroy();
				vertexBuffer.Create( vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
				m_vertexCount[ _index ] = imDrawData->TotalVtxCount;
				vertexBuffer.Map();
			}

			// Index buffer
			Buffer& indexBuffer = m_indexBuffers[ _index ];
			if ( ( indexBuffer.GetBuffer() == VK_NULL_HANDLE ) || ( m_indexCount[ _index ] < imDrawData->TotalIdxCount ) )
			{
				indexBuffer.Unmap();
				indexBuffer.Destroy();
				indexBuffer.Create( indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
				m_indexCount[ _index ] = imDrawData->TotalIdxCount;
				indexBuffer.Map();
			}

			// Upload data
			ImDrawVert* vtxDst = ( ImDrawVert* ) vertexBuffer.GetMappedData();
			ImDrawIdx* idxDst = ( ImDrawIdx* ) indexBuffer.GetMappedData();

			for ( int n = 0; n < imDrawData->CmdListsCount; n++ )
			{
				const ImDrawList* cmd_list = imDrawData->CmdLists[ n ];
				memcpy( vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof( ImDrawVert ) );
				memcpy( idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof( ImDrawIdx ) );
				vtxDst += cmd_list->VtxBuffer.Size;
				idxDst += cmd_list->IdxBuffer.Size;
			}

			// Flush to make writes visible to GPU
			vertexBuffer.Flush();
			indexBuffer.Flush();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::DrawFrame( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		ImDrawData* imDrawData = ImGui::GetDrawData();
		if ( imDrawData && imDrawData->CmdListsCount > 0 )
		{
			ImGuiIO& io = ImGui::GetIO();

			vkCmdBindPipeline( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline );

			// Bind vertex and index buffer
			VkDeviceSize offsets[ 1 ] = { 0 };
			std::vector<VkBuffer> buffers = { m_vertexBuffers[ _index ].GetBuffer() };

			vkCmdBindVertexBuffers( _commandBuffer, 0, static_cast< uint32_t >( buffers.size() ), buffers.data(), offsets );
			vkCmdBindIndexBuffer( _commandBuffer, m_indexBuffers[ _index ].GetBuffer(), 0, VK_INDEX_TYPE_UINT16 );

			// Viewport
			VkViewport viewport{};
			viewport.width = ImGui::GetIO().DisplaySize.x;
			viewport.height = ImGui::GetIO().DisplaySize.y;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport( _commandBuffer, 0, 1, &viewport );

			// UI scale and translate via push constants
			m_pushConstBlock.scale = glm::vec2( 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y );
			m_pushConstBlock.translate = glm::vec2( -1.0f );
			vkCmdPushConstants( _commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( PushConstBlock ), &m_pushConstBlock );

			// Render commands
			int32_t vertexOffset = 0;
			int32_t indexOffset = 0;
			for ( int32_t i = 0; i < imDrawData->CmdListsCount; i++ )
			{
				const ImDrawList* cmd_list = imDrawData->CmdLists[ i ];
				for ( int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++ )
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[ j ];

					const size_t textureID = ( size_t ) ( pcmd->TextureId );

					// Bind imgui pipeline and Descriptors sets
					switch ( textureID )
					{
					case 42:
						vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[ 1 ], 0, nullptr ); // Icons drawing
						break;
					case 12:
						vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[ 2 ], 0, nullptr ); // game drawing
						break;
					default:
						vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[ 0 ], 0, nullptr ); // regular drawing			
						break;
					}

					VkRect2D scissorRect;
					scissorRect.offset.x = std::max( ( int32_t ) ( pcmd->ClipRect.x ), 0 );
					scissorRect.offset.y = std::max( ( int32_t ) ( pcmd->ClipRect.y ), 0 );
					scissorRect.extent.width = ( uint32_t ) ( pcmd->ClipRect.z - pcmd->ClipRect.x );
					scissorRect.extent.height = ( uint32_t ) ( pcmd->ClipRect.w - pcmd->ClipRect.y );
					vkCmdSetScissor( _commandBuffer, 0, 1, &scissorRect );
					vkCmdDrawIndexed( _commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0 );
					indexOffset += pcmd->ElemCount;


				}
				vertexOffset += cmd_list->VtxBuffer.Size;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::InitImgui( GLFWwindow* _window, VkExtent2D _extent )
	{
		// Color scheme
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.f );

		// Dimensions
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( static_cast< float >( _extent.width ), static_cast< float >( _extent.height ) );
		io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );
		m_vertShader = new Shader( m_device );
		m_vertShader->Create( "code/shaders/imgui.vert" );
		m_fragShader = new Shader( m_device );
		m_fragShader->Create( "code/shaders/imgui.frag" );

		// Setup back-end capabilities flags
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
																	  // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		io.KeyMap[ ImGuiKey_Tab ] = GLFW_KEY_TAB;
		io.KeyMap[ ImGuiKey_LeftArrow ] = GLFW_KEY_LEFT;
		io.KeyMap[ ImGuiKey_RightArrow ] = GLFW_KEY_RIGHT;
		io.KeyMap[ ImGuiKey_UpArrow ] = GLFW_KEY_UP;
		io.KeyMap[ ImGuiKey_DownArrow ] = GLFW_KEY_DOWN;
		io.KeyMap[ ImGuiKey_PageUp ] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ ImGuiKey_PageDown ] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ ImGuiKey_Home ] = GLFW_KEY_HOME;
		io.KeyMap[ ImGuiKey_End ] = GLFW_KEY_END;
		io.KeyMap[ ImGuiKey_Insert ] = GLFW_KEY_INSERT;
		io.KeyMap[ ImGuiKey_Delete ] = GLFW_KEY_DELETE;
		io.KeyMap[ ImGuiKey_Backspace ] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ ImGuiKey_Space ] = GLFW_KEY_SPACE;
		io.KeyMap[ ImGuiKey_Enter ] = GLFW_KEY_ENTER;
		io.KeyMap[ ImGuiKey_Escape ] = GLFW_KEY_ESCAPE;
		io.KeyMap[ ImGuiKey_A ] = GLFW_KEY_Q;
		io.KeyMap[ ImGuiKey_C ] = GLFW_KEY_C;
		io.KeyMap[ ImGuiKey_V ] = GLFW_KEY_V;
		io.KeyMap[ ImGuiKey_X ] = GLFW_KEY_X;
		io.KeyMap[ ImGuiKey_Y ] = GLFW_KEY_Y;
		io.KeyMap[ ImGuiKey_Z ] = GLFW_KEY_W;

		io.SetClipboardTextFn = SetClipboardText;
		io.GetClipboardTextFn = GetClipboardText;
		io.ClipboardUserData = _window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::CreateFontAndSampler()
	{
		ImGui::GetIO().Fonts->AddFontFromFileTTF( RenderGlobal::s_defaultImguiFont, 15 );

		// Create font texture
		unsigned char* fontData;
		int texWidth, texHeight;
		ImGui::GetIO().Fonts->GetTexDataAsRGBA32( &fontData, &texWidth, &texHeight );

		m_fontTexture->SetData( fontData, texWidth, texHeight, 1 );
		m_iconsTexture->LoadFromFile( RenderGlobal::s_defaultIcons );
		m_sampler->CreateSampler( 0, 1.f, VK_FILTER_LINEAR );
		m_iconsSampler->CreateSampler( 0, 0.f, VK_FILTER_NEAREST );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::CreateDescriptors()
	{
		Debug::Log() << "ImGui pipeline : create descriptors" << Debug::Endl();
		// Descriptor pool
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSize.descriptorCount = 3;

		std::vector<VkDescriptorPoolSize> poolSizes = { descriptorPoolSize };

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};

		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast< uint32_t >( poolSizes.size() );
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = 3;

		vkCreateDescriptorPool( m_device.vkDevice, &descriptorPoolInfo, nullptr, &m_descriptorPool );

		// Descriptor set layout
		VkDescriptorSetLayoutBinding setLayoutBinding{};
		setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBinding.binding = 0;
		setLayoutBinding.descriptorCount = 1;

		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = { setLayoutBinding };

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
		descriptorSetLayoutCreateInfo.bindingCount = static_cast< uint32_t >( setLayoutBindings.size() );

		vkCreateDescriptorSetLayout( m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout );

		// Descriptor set
		VkDescriptorSetLayout layouts[ 3 ] = { m_descriptorSetLayout ,m_descriptorSetLayout, m_descriptorSetLayout };
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = layouts;
		descriptorSetAllocateInfo.descriptorSetCount = 3;

		vkAllocateDescriptorSets( m_device.vkDevice, &descriptorSetAllocateInfo, m_descriptorSets );

		VkDescriptorImageInfo fontDescriptorImageInfo{};
		fontDescriptorImageInfo.sampler = m_sampler->GetSampler();
		fontDescriptorImageInfo.imageView = m_fontTexture->GetImageView();
		fontDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo iconsDescriptorImageInfo{};
		iconsDescriptorImageInfo.sampler = m_iconsSampler->GetSampler();
		iconsDescriptorImageInfo.imageView = m_iconsTexture->GetImageView();
		iconsDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo view3DDescriptorImageInfo{};
		view3DDescriptorImageInfo.sampler = m_iconsSampler->GetSampler();
		view3DDescriptorImageInfo.imageView = m_gameImageView->GetImageView();
		view3DDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSet{};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = m_descriptorSets[ 0 ];
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.dstBinding = 0;
		writeDescriptorSet.pImageInfo = &fontDescriptorImageInfo;
		writeDescriptorSet.descriptorCount = 1;

		VkWriteDescriptorSet writeDescriptorSetIcons{};
		writeDescriptorSetIcons.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSetIcons.dstSet = m_descriptorSets[ 1 ];
		writeDescriptorSetIcons.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSetIcons.dstBinding = 0;
		writeDescriptorSetIcons.pImageInfo = &iconsDescriptorImageInfo;
		writeDescriptorSetIcons.descriptorCount = 1;

		VkWriteDescriptorSet writeDescriptorSet3DView{};
		writeDescriptorSet3DView.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet3DView.dstSet = m_descriptorSets[ 2 ];
		writeDescriptorSet3DView.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet3DView.dstBinding = 0;
		writeDescriptorSet3DView.pImageInfo = &view3DDescriptorImageInfo;
		writeDescriptorSet3DView.descriptorCount = 1;

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = { writeDescriptorSet, writeDescriptorSetIcons, writeDescriptorSet3DView };

		vkUpdateDescriptorSets( m_device.vkDevice, static_cast< uint32_t >( writeDescriptorSets.size() ), writeDescriptorSets.data(), 0, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::UpdateGameImageDescriptor()
	{
		VkDescriptorImageInfo viewGameDescriptorImageInfo{};
		viewGameDescriptorImageInfo.sampler = m_iconsSampler->GetSampler();
		viewGameDescriptorImageInfo.imageView = m_gameImageView->GetImageView();
		viewGameDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSet3DView{};
		writeDescriptorSet3DView.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet3DView.dstSet = m_descriptorSets[ 2 ];
		writeDescriptorSet3DView.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet3DView.dstBinding = 0;
		writeDescriptorSet3DView.pImageInfo = &viewGameDescriptorImageInfo;
		writeDescriptorSet3DView.descriptorCount = 1;

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = { writeDescriptorSet3DView };

		vkUpdateDescriptorSets( m_device.vkDevice, static_cast< uint32_t >( writeDescriptorSets.size() ), writeDescriptorSets.data(), 0, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ImguiPipeline::CreateGraphicsPipeline( VkRenderPass _renderPass )
	{
		// Pipeline cache
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		vkCreatePipelineCache( m_device.vkDevice, &pipelineCacheCreateInfo, nullptr, &m_pipelineCache );

		// Push constants for UI rendering parameters
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof( PushConstBlock );

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		vkCreatePipelineLayout( m_device.vkDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout );

		// Setup graphics pipeline for UI rendering
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyState.flags = 0;
		inputAssemblyState.primitiveRestartEnable = VK_FALSE;

		VkPipelineRasterizationStateCreateInfo rasterizationState{};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.flags = 0;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.lineWidth = 1.0f;

		// Enable blending
		VkPipelineColorBlendAttachmentState blendAttachmentState = {};
		blendAttachmentState.blendEnable = VK_TRUE;
		blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &blendAttachmentState;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.front = depthStencilState.back;
		depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		viewportState.flags = 0;

		VkPipelineMultisampleStateCreateInfo multisampleState{};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.flags = 0;

		std::vector<VkDynamicState> dynamicStateEnables =
		{
			VK_DYNAMIC_STATE_VIEWPORT
			, VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = static_cast< uint32_t >( dynamicStateEnables.size() );
		dynamicState.flags = 0;

		// Vertex shader
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_vertShader->GetModule();
		vertShaderStageInfo.pName = "main";

		// Fragment shader
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_fragShader->GetModule();
		fragShaderStageInfo.pName = "main";

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

		// Vertex bindings an attributes based on ImGui vertex definition
		VkVertexInputBindingDescription vInputBindDescription{};
		vInputBindDescription.binding = 0;
		vInputBindDescription.stride = sizeof( ImDrawVert );
		vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		std::vector<VkVertexInputBindingDescription> vertexInputBindings = { vInputBindDescription };

		// Location 0: Position
		VkVertexInputAttributeDescription vInputAttribDescriptionPosition = {};
		vInputAttribDescriptionPosition.location = 0;
		vInputAttribDescriptionPosition.binding = 0;
		vInputAttribDescriptionPosition.format = VK_FORMAT_R32G32_SFLOAT;
		vInputAttribDescriptionPosition.offset = offsetof( ImDrawVert, pos );

		// Location 1: UV
		VkVertexInputAttributeDescription vInputAttribDescriptionUV{};
		vInputAttribDescriptionUV.location = 1;
		vInputAttribDescriptionUV.binding = 0;
		vInputAttribDescriptionUV.format = VK_FORMAT_R32G32_SFLOAT;
		vInputAttribDescriptionUV.offset = offsetof( ImDrawVert, uv );

		// Location 2: Color
		VkVertexInputAttributeDescription vInputAttribDescriptionColor{};
		vInputAttribDescriptionColor.location = 2;
		vInputAttribDescriptionColor.binding = 0;
		vInputAttribDescriptionColor.format = VK_FORMAT_R8G8B8A8_UNORM;
		vInputAttribDescriptionColor.offset = offsetof( ImDrawVert, col );

		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes =
		{
			vInputAttribDescriptionPosition
			, vInputAttribDescriptionUV
			, vInputAttribDescriptionColor
		};

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = static_cast< uint32_t >( vertexInputBindings.size() );
		vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
		vertexInputState.vertexAttributeDescriptionCount = static_cast< uint32_t >( vertexInputAttributes.size() );
		vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.layout = m_pipelineLayout;
		pipelineCreateInfo.renderPass = _renderPass;
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.basePipelineIndex = -1;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.stageCount = static_cast< uint32_t >( shaderStages.size() );
		pipelineCreateInfo.pStages = shaderStages.data();
		pipelineCreateInfo.pVertexInputState = &vertexInputState;
		pipelineCreateInfo.subpass = 0;

		vkCreateGraphicsPipelines( m_device.vkDevice, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipeline );
	}
}