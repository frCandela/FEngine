#include "render/draw/fanDrawImgui.hpp"

#include <array>
#include "imgui/imgui.h"
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanRenderPass.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::Create( Device& _device,
                            const int _swapchainImagesCount,
                            VkRenderPass _renderPass,
                            GLFWwindow* _window,
                            VkExtent2D _extent,
                            ImageView& _gameImageView )
    {
        mVertexBuffers.resize( _swapchainImagesCount );
        mIndexBuffers.resize( _swapchainImagesCount );
        mVertexCount.resize( _swapchainImagesCount, 0 );
        mIndexCount.resize( _swapchainImagesCount, 0 );

        ImGui::CreateContext();

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        InitImgui( _device, _window, _extent );

        // create font and sampler
        ImGui::GetIO().Fonts->AddFontFromFileTTF( Path::Normalize( RenderGlobal::sDefaultImguiFont ).c_str(), 15 );
        unsigned char* fontData;
        int texWidth, texHeight;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32( &fontData, &texWidth, &texHeight );
        mTextureFont.LoadFromPixels( fontData, { (uint32_t)texWidth, (uint32_t)texHeight }, 1 );
        mTextureFont.Create( _device );
        mTextureIcons.LoadFromFile( RenderGlobal::sDefaultIcons );
        mTextureIcons.Create( _device );
        mSampler.Create( _device, 0, 1.f, VK_FILTER_LINEAR );
        mSamplerIcons.Create( _device, 0, 0.f, VK_FILTER_NEAREST );

        // create descriptors
        VkImageView views[3]    = { mTextureFont.mImageView,
                                    mTextureIcons.mImageView,
                                    _gameImageView.mImageView };
        VkSampler   samplers[3] = { mSampler.mSampler, mSamplerIcons.mSampler, mSamplerIcons.mSampler };
        mDescriptorImages.Create( _device, views, 3, samplers );

        const VkExtent2D extent = { (uint32_t)ImGui::GetIO().DisplaySize.x,
                                    (uint32_t)ImGui::GetIO().DisplaySize.y };
        mPipeline.Create( _device, GetPipelineConfig(), extent, _renderPass, true );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::Destroy( Device& _device )
    {
        mTextureFont.Destroy( _device );
        mTextureIcons.Destroy( _device );
        mSampler.Destroy( _device );
        mSamplerIcons.Destroy( _device );
        mFragmentShader.Destroy( _device );
        mVertexShader.Destroy( _device );

        mPipeline.Destroy( _device );
        mDescriptorImages.Destroy( _device );

        for( Buffer& buffer : mVertexBuffers )
        {
            buffer.Destroy( _device );
        }
        mVertexBuffers.clear();

        for( Buffer& buffer : mIndexBuffers )
        {
            buffer.Destroy( _device );
        }
        mIndexBuffers.clear();

        ImGui::DestroyContext();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::ReloadIcons( Device& _device )
    {
        Debug::Log( "reloading icons" );

        mTextureIcons.Destroy( _device );
        mTextureIcons.LoadFromFile( RenderGlobal::sDefaultIcons );
        mTextureIcons.Create( _device );
        mDescriptorImages.UpdateDescriptorSet( _device, 1, mTextureIcons.mImageView, mSamplerIcons.mSampler );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::UpdateBuffer( Device& _device, const size_t _index )
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();

        // Update buffers only if the imgui command list is not empty
        if( imDrawData && imDrawData->CmdListsCount > 0 )
        {
            // Note: Alignment is done inside buffer creation
            VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof( ImDrawVert );
            VkDeviceSize indexBufferSize  = imDrawData->TotalIdxCount * sizeof( ImDrawIdx );

            // Update buffers only if vertex or index count has been changed compared to current buffer size

            // Vertex buffer
            Buffer& vertexBuffer = mVertexBuffers[_index];
            if( ( vertexBuffer.mBuffer == VK_NULL_HANDLE ) ||
                ( mVertexCount[_index] != imDrawData->TotalVtxCount ) )
            {
                vertexBuffer.Unmap( _device );
                vertexBuffer.Destroy( _device );
                vertexBuffer.Create( _device, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
                mVertexCount[_index] = imDrawData->TotalVtxCount;
                vertexBuffer.Map( _device );
                _device.AddDebugName( (uint64_t)vertexBuffer.mBuffer, "imgui vertex buffer" );
                _device.AddDebugName( (uint64_t)vertexBuffer.mMemory, "imgui vertex buffer" );
            }

            // Index buffer
            Buffer& indexBuffer = mIndexBuffers[_index];
            if( ( indexBuffer.mBuffer == VK_NULL_HANDLE ) ||
                ( mIndexCount[_index] < imDrawData->TotalIdxCount ) )
            {
                indexBuffer.Unmap( _device );
                indexBuffer.Destroy( _device );
                indexBuffer.Create( _device, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
                mIndexCount[_index] = imDrawData->TotalIdxCount;
                indexBuffer.Map( _device );
                _device.AddDebugName( (uint64_t)indexBuffer.mBuffer, "imgui index buffer" );
                _device.AddDebugName( (uint64_t)indexBuffer.mMemory, "imgui index buffer" );
            }

            // Upload data
            ImDrawVert* vtxDst = (ImDrawVert*)vertexBuffer.mMappedData;
            ImDrawIdx * idxDst = (ImDrawIdx*)indexBuffer.mMappedData;

            for( int n = 0; n < imDrawData->CmdListsCount; n++ )
            {
                const ImDrawList* cmd_list = imDrawData->CmdLists[n];
                memcpy( vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof( ImDrawVert ) );
                memcpy( idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof( ImDrawIdx ) );
                vtxDst += cmd_list->VtxBuffer.Size;
                idxDst += cmd_list->IdxBuffer.Size;
            }

            // Flush to make writes visible to GPU
            vertexBuffer.Flush( _device );
            indexBuffer.Flush( _device );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::DrawFrame( VkCommandBuffer _commandBuffer, const size_t _index )
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        if( imDrawData && imDrawData->CmdListsCount > 0 )
        {
            ImGuiIO& io = ImGui::GetIO();

            const VkExtent2D extent = { (uint32_t)ImGui::GetIO().DisplaySize.x,
                                        (uint32_t)ImGui::GetIO().DisplaySize.y };
            mPipeline.Bind( _commandBuffer, extent );

            // Bind vertex and index buffer
            VkDeviceSize          offsets[1] = { 0 };
            std::vector<VkBuffer> buffers    = { mVertexBuffers[_index].mBuffer };

            vkCmdBindVertexBuffers( _commandBuffer,
                                    0,
                                    static_cast< uint32_t >( buffers.size() ),
                                    buffers.data(),
                                    offsets );
            vkCmdBindIndexBuffer( _commandBuffer, mIndexBuffers[_index].mBuffer, 0, VK_INDEX_TYPE_UINT16 );

            // UI scale and translate via push constants
            mPushConstBlock.scale     = glm::vec2( 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y );
            mPushConstBlock.translate = glm::vec2( -1.0f );
            vkCmdPushConstants( _commandBuffer,
                                mPipeline.mPipelineLayout,
                                VK_SHADER_STAGE_VERTEX_BIT,
                                0,
                                sizeof( PushConstBlock ),
                                &mPushConstBlock );

            // Render commands
            int32_t      vertexOffset = 0;
            int32_t      indexOffset  = 0;
            for( int32_t i            = 0; i < imDrawData->CmdListsCount; i++ )
            {
                const ImDrawList* cmd_list = imDrawData->CmdLists[i];
                for( int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++ )
                {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

                    const size_t textureID = (size_t)( pcmd->TextureId );

                    // Bind pipeline and descriptors @todo, replace the numbers with something more explicit
                    switch( textureID )
                    {
                        case 42:
                            vkCmdBindDescriptorSets( _commandBuffer,
                                                     VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                     mPipeline.mPipelineLayout,
                                                     0,
                                                     1,
                                                     &mDescriptorImages.mDescriptorSets[1],
                                                     0,
                                                     nullptr ); // Icons drawing
                            break;
                        case 12:
                            vkCmdBindDescriptorSets( _commandBuffer,
                                                     VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                     mPipeline.mPipelineLayout,
                                                     0,
                                                     1,
                                                     &mDescriptorImages.mDescriptorSets[2],
                                                     0,
                                                     nullptr ); // game drawing
                            break;
                        default:
                            vkCmdBindDescriptorSets( _commandBuffer,
                                                     VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                     mPipeline.mPipelineLayout,
                                                     0,
                                                     1,
                                                     &mDescriptorImages.mDescriptorSets[0],
                                                     0,
                                                     nullptr ); // regular drawing
                            break;
                    }

                    VkRect2D scissorRect;
                    scissorRect.offset.x      = std::max( (int32_t)( pcmd->ClipRect.x ), 0 );
                    scissorRect.offset.y      = std::max( (int32_t)( pcmd->ClipRect.y ), 0 );
                    scissorRect.extent.width  = (uint32_t)( pcmd->ClipRect.z - pcmd->ClipRect.x );
                    scissorRect.extent.height = (uint32_t)( pcmd->ClipRect.w - pcmd->ClipRect.y );
                    vkCmdSetScissor( _commandBuffer, 0, 1, &scissorRect ); // @hack
                    vkCmdDrawIndexed( _commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0 );
                    indexOffset += pcmd->ElemCount;
                }
                vertexOffset += cmd_list->VtxBuffer.Size;
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::InitImgui( Device& _device, GLFWwindow* _window, VkExtent2D _extent )
    {
        // Color scheme
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TitleBg] = ImVec4( 50.f / 255.f, 50.f / 255.f, 50.f / 255.f, 1.f );

        // Dimensions
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize             = ImVec2( static_cast< float >( _extent.width ),
                                             static_cast< float >( _extent.height ) );
        io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );
        mVertexShader.Create( _device, RenderGlobal::sImguiVertexShader );
        mFragmentShader.Create( _device, RenderGlobal::sImguiFragmentShader );

        // Setup back-end capabilities flags
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab]        = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow]  = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow]    = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow]  = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp]     = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown]   = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home]       = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End]        = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert]     = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete]     = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace]  = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space]      = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter]      = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape]     = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A]          = GLFW_KEY_Q;
        io.KeyMap[ImGuiKey_C]          = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V]          = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X]          = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y]          = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z]          = GLFW_KEY_W;

        io.SetClipboardTextFn = SetClipboardText;
        io.GetClipboardTextFn = GetClipboardText;
        io.ClipboardUserData  = _window;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::RecordCommandBuffer( const size_t _index,
                                         Device& _device,
                                         RenderPass& _renderPass,
                                         FrameBuffer& _framebuffer )
    {
        UpdateBuffer( _device, _index );

        VkCommandBuffer                commandBuffer                = mCommandBuffers.mBuffers[_index];
        VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = CommandBuffer::GetInheritanceInfo(
                _renderPass.mRenderPass,
                _framebuffer.mFrameBuffers[_index] );
        VkCommandBufferBeginInfo       commandBufferBeginInfo       =
                                               CommandBuffer::GetBeginInfo( &commandBufferInheritanceInfo );

        if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
        {
            DrawFrame( commandBuffer, _index );

            if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
            {
                Debug::Error() << "Could not record command buffer " << _index << "." << Debug::Endl();
            }
        }
        else
        {
            Debug::Error() << "Could not record command buffer " << _index << "." << Debug::Endl();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::UpdateGameImageDescriptor( Device& _device, ImageView& _gameImageView )
    {
        mDescriptorImages.UpdateDescriptorSet( _device, 2, _gameImageView.mImageView, mSamplerIcons.mSampler );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    PipelineConfig DrawImgui::GetPipelineConfig() const
    {
        PipelineConfig config( mVertexShader, mFragmentShader );

        config.pushConstantRanges                           = { { VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( PushConstBlock ) } };
        config.descriptorSetLayouts                         = { mDescriptorImages.mDescriptorSetLayout };
        config.rasterizationStateInfo.cullMode              = VK_CULL_MODE_NONE;
        config.attachmentBlendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        config.depthStencilStateInfo.depthTestEnable        = VK_FALSE;
        config.depthStencilStateInfo.depthWriteEnable       = VK_FALSE;
        config.depthStencilStateInfo.depthCompareOp         = VK_COMPARE_OP_LESS_OR_EQUAL;
        config.depthStencilStateInfo.front                  = config.depthStencilStateInfo.back;
        config.depthStencilStateInfo.back.compareOp         = VK_COMPARE_OP_ALWAYS;

        // Vertex bindings an attributes based on ImGui vertex definition
        VkVertexInputBindingDescription vInputBindDescription {};
        vInputBindDescription.binding   = 0;
        vInputBindDescription.stride    = sizeof( ImDrawVert );
        vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        config.bindingDescription       = { vInputBindDescription };

        // Location 0: Position
        VkVertexInputAttributeDescription vInputAttribDescriptionPosition = {};
        vInputAttribDescriptionPosition.location = 0;
        vInputAttribDescriptionPosition.binding  = 0;
        vInputAttribDescriptionPosition.format   = VK_FORMAT_R32G32_SFLOAT;
        vInputAttribDescriptionPosition.offset   = offsetof( ImDrawVert, pos );

        // Location 1: UV
        VkVertexInputAttributeDescription vInputAttribDescriptionUV {};
        vInputAttribDescriptionUV.location = 1;
        vInputAttribDescriptionUV.binding  = 0;
        vInputAttribDescriptionUV.format   = VK_FORMAT_R32G32_SFLOAT;
        vInputAttribDescriptionUV.offset   = offsetof( ImDrawVert, uv );

        // Location 2: Color
        VkVertexInputAttributeDescription vInputAttribDescriptionColor {};
        vInputAttribDescriptionColor.location = 2;
        vInputAttribDescriptionColor.binding  = 0;
        vInputAttribDescriptionColor.format   = VK_FORMAT_R8G8B8A8_UNORM;
        vInputAttribDescriptionColor.offset   = offsetof( ImDrawVert, col );

        config.attributeDescriptions = {
                vInputAttribDescriptionPosition, vInputAttribDescriptionUV, vInputAttribDescriptionColor
        };

        return config;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DrawImgui::SetClipboardText( void* _userData, const char* _text )
    {
        glfwSetClipboardString( (GLFWwindow*)_userData, _text );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const char* DrawImgui::GetClipboardText( void* _userData )
    {
        return glfwGetClipboardString( (GLFWwindow*)_userData );
    }
}