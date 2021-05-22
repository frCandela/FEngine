#include "render/fanRenderer.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/resources/fanResourceManager.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/core/fanInstance.hpp"
#include "render/fanWindow.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Renderer::Renderer( Window& _window, ResourceManager& _resourceManager, const ViewType _viewType ) :
            mWindow( _window ),
            mResourceManager( _resourceManager ),
            mDevice( _window.mDevice ),
            mViewType( _viewType )
    {
        const uint32_t imagesCount = mWindow.mSwapchain.mImagesCount;
        mGameExtent = mWindow.mSwapchain.mExtent;
        RenderPass& finalRenderPass = ( mViewType == ViewType::Editor
                ? mRenderPassImgui
                : mRenderPassPostprocess );

        CreateRenderPasses();

        mSamplerGameColor.Create( mDevice, 0, 1.f, VK_FILTER_LINEAR );
        mSamplerPostprocessColor.Create( mDevice, 0, 1.f, VK_FILTER_LINEAR );
        CreateFramebuffers( mWindow.mSwapchain.mExtent );
        mFramebuffersSwapchain.CreateForSwapchain( mDevice,
                                                   imagesCount,
                                                   mWindow.mSwapchain.mExtent,
                                                   finalRenderPass,
                                                   mWindow.mSwapchain.mImageViews );

        mTextureManager.CreateNewTextures( mDevice );
        CreateTextureDescriptor();
        CreateShaders();

        mDrawDebug.Create( mDevice, imagesCount );
        mDrawUI.Create( mDevice, imagesCount );
        mDrawModels.Create( mDevice, imagesCount );
        mDrawPostprocess.Create( mDevice, imagesCount, mImageViewGameColor );
        mDrawImgui.Create( mDevice,
                           imagesCount,
                           finalRenderPass.mRenderPass,
                           mWindow.mWindow,
                           mWindow.mSwapchain.mExtent,
                           mImageViewPostprocessColor );

        CreatePipelines();
        CreateCommandBuffers();
        RecordAllCommandBuffers();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Renderer::~Renderer()
    {
        vkDeviceWaitIdle( mDevice.mDevice );

        mDrawImgui.Destroy( mDevice );
        mDrawModels.Destroy( mDevice );
        mDrawUI.mPipeline.Destroy( mDevice );
        mDrawDebug.Destroy( mDevice );
        mDrawUI.Destroy( mDevice );
        mDrawPostprocess.Destroy( mDevice );

        mMeshManager.Clear( mDevice );
        mMesh2DManager.Clear( mDevice );
        mTextureManager.Clear( mDevice );
        mDescriptorTextures.Destroy( mDevice );

        DestroyShaders();

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
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::CreateShaders()
    {
        mDrawDebug.mVertexShaderLines.Create( mDevice, "shaders/debugLines.vert" );
        mDrawDebug.mFragmentShaderLines.Create( mDevice, "shaders/debugLines.frag" );
        mDrawDebug.mVertexShaderLinesNDT.Create( mDevice, "shaders/debugLines.vert" );
        mDrawDebug.mFragmentShaderLinesNDT.Create( mDevice, "shaders/debugLines.frag" );
        mDrawDebug.mVertexShaderTriangles.Create( mDevice, "shaders/debugTriangles.vert" );
        mDrawDebug.mFragmentShaderTriangles.Create( mDevice, "shaders/debugTriangles.frag" );
        mDrawDebug.mVertexShaderLines2D.Create( mDevice, "shaders/debugLines2D.vert" );
        mDrawDebug.mFragmentShaderLines2D.Create( mDevice, "shaders/debugLines2D.frag" );
        mDrawModels.mVertexShader.Create( mDevice, "shaders/models.vert" );
        mDrawModels.mFragmentShader.Create( mDevice, "shaders/models.frag" );
        mDrawUI.mVertexShader.Create( mDevice, "shaders/ui.vert" );
        mDrawUI.mFragmentShader.Create( mDevice, "shaders/ui.frag" );
        mDrawPostprocess.mVertexShader.Create( mDevice, "shaders/postprocess.vert" );
        mDrawPostprocess.mFragmentShader.Create( mDevice, "shaders/postprocess.frag" );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::DestroyShaders()
    {
        mDrawDebug.mVertexShaderLines.Destroy( mDevice );
        mDrawDebug.mFragmentShaderLines.Destroy( mDevice );
        mDrawDebug.mVertexShaderLinesNDT.Destroy( mDevice );
        mDrawDebug.mFragmentShaderLinesNDT.Destroy( mDevice );
        mDrawDebug.mVertexShaderTriangles.Destroy( mDevice );
        mDrawDebug.mFragmentShaderTriangles.Destroy( mDevice );
        mDrawDebug.mVertexShaderLines2D.Destroy( mDevice );
        mDrawDebug.mFragmentShaderLines2D.Destroy( mDevice );
        mDrawModels.mVertexShader.Destroy( mDevice );
        mDrawModels.mFragmentShader.Destroy( mDevice );
        mDrawUI.mVertexShader.Destroy( mDevice );
        mDrawUI.mFragmentShader.Destroy( mDevice );
        mDrawPostprocess.mVertexShader.Destroy( mDevice );
        mDrawPostprocess.mFragmentShader.Destroy( mDevice );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::CreatePipelines()
    {
        const PipelineConfig debugLinesPipelineConfig            = mDrawDebug.GetPipelineConfigLines();
        const PipelineConfig debugLinesNoDepthTestPipelineConfig = mDrawDebug.GetPipelineConfigLinesNDT();
        const PipelineConfig debugTrianglesPipelineConfig        = mDrawDebug.GetPipelineConfigTriangles();
        const PipelineConfig debugLines2DPipelineConfig          = mDrawDebug.GetPipelineConfigLines2D();
        const PipelineConfig ppPipelineConfig                    = mDrawPostprocess.GetPipelineConfig();
        const PipelineConfig modelsPipelineConfig                = mDrawModels.GetPipelineConfig( mDescriptorTextures );
        const PipelineConfig uiPipelineConfig                    = mDrawUI.GetPipelineConfig( mDescriptorTextures );

        mDrawDebug.mPipelineLines.Create( mDevice,
                                          debugLinesPipelineConfig,
                                          mGameExtent,
                                          mRenderPassGame.mRenderPass );
        mDrawDebug.mPipelineLinesNDT.Create( mDevice,
                                             debugLinesNoDepthTestPipelineConfig,
                                             mGameExtent,
                                             mRenderPassGame.mRenderPass );
        mDrawDebug.mPipelineTriangles.Create( mDevice,
                                              debugTrianglesPipelineConfig,
                                              mGameExtent,
                                              mRenderPassGame.mRenderPass );
        mDrawModels.mPipeline.Create( mDevice,
                                      modelsPipelineConfig,
                                      mGameExtent,
                                      mRenderPassGame.mRenderPass );
        mDrawDebug.mPipelineLines2D.Create( mDevice,
                                            debugLines2DPipelineConfig,
                                            mGameExtent,
                                            mRenderPassPostprocess.mRenderPass );
        mDrawUI.mPipeline.Create( mDevice,
                                  uiPipelineConfig,
                                  mGameExtent,
                                  mRenderPassPostprocess.mRenderPass );
        mDrawPostprocess.mPipeline.Create( mDevice,
                                           ppPipelineConfig,
                                           mGameExtent,
                                           mRenderPassPostprocess.mRenderPass );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::DestroyPipelines()
    {
        mDrawPostprocess.mPipeline.Destroy( mDevice );
        mDrawModels.mPipeline.Destroy( mDevice );
        mDrawUI.mPipeline.Destroy( mDevice );
        mDrawDebug.mPipelineLines.Destroy( mDevice );
        mDrawDebug.mPipelineLinesNDT.Destroy( mDevice );
        mDrawDebug.mPipelineTriangles.Destroy( mDevice );
        mDrawDebug.mPipelineLines2D.Destroy( mDevice );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::DrawFrame()
    {
        SCOPED_PROFILE( draw_frame );

        bool textureCreated = false;

        const VkResult result = mWindow.mSwapchain.AcquireNextImage( mDevice );
        if( result == VK_ERROR_OUT_OF_DATE_KHR )
        {
            // window minimized
            if( mWindow.GetExtent().width == 0 && mWindow.GetExtent().height == 0 )
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
        else if( result != VK_SUCCESS )
        {
            Debug::Error( "Could not acquire next image" );
        }
        else
        {
            vkWaitForFences( mDevice.mDevice,
                             1,
                             mWindow.mSwapchain.GetCurrentInFlightFence(),
                             VK_TRUE,
                             std::numeric_limits<uint64_t>::max() );
            vkResetFences( mDevice.mDevice, 1, mWindow.mSwapchain.GetCurrentInFlightFence() );

            mMeshManager.DestroyRemovedMeshes( mDevice );
            mMesh2DManager.DestroyRemovedMeshes( mDevice );
            mTextureManager.DestroyRemovedTextures( mDevice );

            mMeshManager.CreateNewMeshes( mDevice );
            mMesh2DManager.CreateNewMeshes( mDevice );
            textureCreated = mTextureManager.CreateNewTextures( mDevice );
        }

        ImGui::GetIO().DisplaySize = ImVec2( static_cast< float >( mWindow.mSwapchain.mExtent.width ),
                                             static_cast< float >( mWindow.mSwapchain.mExtent.height ) );

        if( textureCreated )
        {
            WaitIdle();
            Debug::Log( "reload textures" );
            CreateTextureDescriptor();
        }

        const uint32_t currentFrame = mWindow.mSwapchain.mCurrentFrame;
        UpdateUniformBuffers( mDevice, currentFrame );
        {
            SCOPED_PROFILE( record_cmd );
            RecordSecondaryCommandBuffers( currentFrame );
            RecordPrimaryCommandBuffer( currentFrame );
        }

        {
            SCOPED_PROFILE( submit );
            SubmitCommandBuffers();
            mWindow.mSwapchain.PresentImage( mDevice );
            mWindow.mSwapchain.StartNextFrame();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::ResizeGame( VkExtent2D _extent )
    {
        WaitIdle();

        mGameExtent = _extent;

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
        CreateFramebuffers( _extent );

        mDrawPostprocess.mDescriptorImage.Destroy( mDevice );
        mDrawPostprocess.mDescriptorImage.Create( mDevice,
                                                  &mImageViewGameColor.mImageView,
                                                  1,
                                                  &mDrawPostprocess.mSampler.mSampler );

        mDrawImgui.UpdateGameImageDescriptor( mDevice, mImageViewPostprocessColor );
        mDrawDebug.mUniformsScreenSize.mScreenSize = { mGameExtent.width, mGameExtent.height };

        RecordAllCommandBuffers();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::ResizeSwapchain()
    {
        WaitIdle();
        const VkExtent2D extent = mWindow.GetExtent();

        if( mViewType == ViewType::Game )
        {
            mGameExtent = extent;
        }

        Debug::Highlight() << "Resize renderer: " << extent.width << "x" << extent.height << Debug::Endl();
        mWindow.mSwapchain.Resize( mDevice, mWindow.mSurface, extent );

        mFramebuffersSwapchain.Destroy( mDevice );
        RenderPass& finalRenderPass = ( mViewType == ViewType::Editor
                ? mRenderPassImgui
                : mRenderPassPostprocess );
        mFramebuffersSwapchain.CreateForSwapchain( mDevice,
                                                   mWindow.mSwapchain.mImagesCount,
                                                   extent,
                                                   finalRenderPass,
                                                   mWindow.mSwapchain.mImageViews );

        if( mViewType == ViewType::Editor )
        {
            RecordAllCommandBuffers();
        }
        else
        {
            ResizeGame( mGameExtent );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::UpdateUniformBuffers( Device& _device, const size_t _index )
    {
        mDrawPostprocess.UpdateUniformBuffers( _device, _index );
        mDrawModels.UpdateUniformBuffers( _device, _index );
        mDrawDebug.UpdateUniformBuffers( _device, _index );
        mDrawUI.UpdateUniformBuffers( _device, _index );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::WaitIdle()
    {
        vkDeviceWaitIdle( mDevice.mDevice );
        Debug::Log( "Renderer idle", Debug::Type::Render );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::SetMainCamera( const glm::mat4 _projection,
                                  const glm::mat4 _view,
                                  const glm::vec3 _position )
    {
        SCOPED_PROFILE( set_main_camera );
        mDrawModels.mUniforms.mUniformsProjView.mView = _view;
        mDrawModels.mUniforms.mUniformsProjView.mProj = _projection;
        mDrawModels.mUniforms.mUniformsProjView.mProj[1][1] *= -1;

        mDrawModels.mUniforms.mUniformsCameraPosition.mCameraPosition = _position;

        mDrawDebug.mUniformsMVPColor.mModel = glm::mat4( 1.0 );
        mDrawDebug.mUniformsMVPColor.mView  = _view;
        mDrawDebug.mUniformsMVPColor.mProj  = mDrawModels.mUniforms.mUniformsProjView.mProj;
        mDrawDebug.mUniformsMVPColor.mColor = glm::vec4( 1, 1, 1, 1 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::SetDirectionalLights( const std::vector<RenderDataDirectionalLight>& _lightData )
    {
        SCOPED_PROFILE( set_dir_lights );
        mDrawModels.SetDirectionalLights( _lightData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::SetPointLights( const std::vector<RenderDataPointLight>& _lightData )
    {
        SCOPED_PROFILE( set_point_lights );
        mDrawModels.SetPointLights( _lightData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::SetDrawData( const std::vector<RenderDataModel>& _drawData )
    {
        SCOPED_PROFILE( set_draw_data );
        mDrawModels.SetDrawData( mDevice, mWindow.mSwapchain.mImagesCount, _drawData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::SetUIDrawData( const std::vector<RenderDataMesh2D>& _drawData )
    {
        SCOPED_PROFILE( set_ui_draw_data );
        mDrawUI.SetUIDrawData( _drawData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::SetDebugDrawData( const std::vector<DebugVertex>& _debugLines,
                                     const std::vector<DebugVertex>& _debugLinesNoDepthTest,
                                     const std::vector<DebugVertex>& _debugTriangles,
                                     const std::vector<DebugVertex2D>& _debugLines2D )
    {
        SCOPED_PROFILE( set_debug_draw_data );
        mDrawDebug.SetDebugDrawData( mWindow.mSwapchain.mCurrentFrame,
                                     mDevice,
                                     _debugLines,
                                     _debugLinesNoDepthTest,
                                     _debugTriangles,
                                     _debugLines2D );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::RecordSecondaryCommandBuffers( const uint32_t _index )
    {
        RenderPass & finalRenderPass  = mViewType == ViewType::Editor
                ? mRenderPassImgui
                : mRenderPassPostprocess;
        FrameBuffer& finalFramebuffer = mViewType == ViewType::Editor
                ? mFramebuffersPostprocess
                : mFramebuffersSwapchain;

        mDrawModels.RecordCommandBuffer( _index,
                                         mRenderPassGame,
                                         mFrameBuffersGame,
                                         mGameExtent,
                                         mDescriptorTextures );
        mDrawDebug.RecordCommandBuffer( _index, mRenderPassGame, mFrameBuffersGame, mGameExtent );
        mDrawDebug.RecordCommandBuffer2D( _index, mRenderPassPostprocess, finalFramebuffer, mGameExtent );
        mDrawUI.RecordCommandBuffer( _index,
                                     mRenderPassPostprocess,
                                     finalFramebuffer,
                                     mGameExtent,
                                     mDescriptorTextures );
        mDrawImgui.RecordCommandBuffer( _index, mDevice, finalRenderPass, mFramebuffersSwapchain );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::RecordAllCommandBuffers()
    {
        for( uint32_t i = 0; i < mWindow.mSwapchain.mImagesCount; i++ )
        {
            FrameBuffer& finalFramebuffer = mViewType == ViewType::Editor
                    ? mFramebuffersPostprocess
                    : mFramebuffersSwapchain;
            mDrawPostprocess.RecordCommandBuffer( i, mRenderPassPostprocess, finalFramebuffer, mGameExtent );
            RecordSecondaryCommandBuffers( i );
            RecordPrimaryCommandBuffer( i );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::RecordPrimaryCommandBuffer( const uint32_t _index )
    {
        SCOPED_PROFILE( primary );
        VkCommandBuffer commandBuffer = mPrimaryCommandBuffers.mBuffers[_index];

        VkCommandBufferBeginInfo commandBufferBeginInfo = CommandBuffer::GetBeginInfo( VK_NULL_HANDLE );

        std::vector<VkClearValue> clearValues( 2 );
        clearValues[0].color        = { mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a };
        clearValues[1].depthStencil = { 1.0f, 0 };

        FrameBuffer& finalFramebuffer = mViewType == ViewType::Editor
                ? mFramebuffersPostprocess
                : mFramebuffersSwapchain;

        VkRenderPassBeginInfo renderPassInfo            = RenderPass::GetBeginInfo(
                mRenderPassGame.mRenderPass,
                mFrameBuffersGame.mFrameBuffers[_index],
                mGameExtent,
                clearValues.data(),
                (uint32_t)clearValues.size() );
        VkRenderPassBeginInfo renderPassInfoPostprocess = RenderPass::GetBeginInfo(
                mRenderPassPostprocess.mRenderPass,
                finalFramebuffer.mFrameBuffers[_index],
                mGameExtent,
                clearValues.data(),
                (uint32_t)clearValues.size() );
        VkRenderPassBeginInfo renderPassInfoImGui       = RenderPass::GetBeginInfo(
                mRenderPassImgui.mRenderPass,
                mFramebuffersSwapchain.mFrameBuffers[_index],
                mWindow.mSwapchain.mExtent,
                clearValues.data(),
                (uint32_t)clearValues.size() );

        if( vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo ) == VK_SUCCESS )
        {
            vkCmdBeginRenderPass( commandBuffer,
                                  &renderPassInfo,
                                  VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
            {
                vkCmdExecuteCommands( commandBuffer, 1, &mDrawModels.mCommandBuffers.mBuffers[_index] );
                if( !mDrawDebug.HasNothingToDraw() )
                {
                    vkCmdExecuteCommands( commandBuffer, 1, &mDrawDebug.mCommandBuffers.mBuffers[_index] );
                }
            }
            vkCmdEndRenderPass( commandBuffer );

            vkCmdBeginRenderPass( commandBuffer,
                                  &renderPassInfoPostprocess,
                                  VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
            {

                vkCmdExecuteCommands( commandBuffer, 1, &mDrawPostprocess.mCommandBuffers.mBuffers[_index] );
                vkCmdExecuteCommands( commandBuffer, 1, &mDrawUI.mCommandBuffers.mBuffers[_index] );
                if( mDrawDebug.mNumLines2D != 0 )
                {
                    vkCmdExecuteCommands( commandBuffer, 1, &mDrawDebug.mCommandBuffers2D.mBuffers[_index] );
                }

                if( mViewType == ViewType::Game )
                {
                    vkCmdExecuteCommands( commandBuffer, 1, &mDrawImgui.mCommandBuffers.mBuffers[_index] );
                }
            }
            vkCmdEndRenderPass( commandBuffer );

            if( mViewType == ViewType::Editor )
            {
                vkCmdBeginRenderPass( commandBuffer,
                                      &renderPassInfoImGui,
                                      VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
                vkCmdExecuteCommands( commandBuffer, 1, &mDrawImgui.mCommandBuffers.mBuffers[_index] );
                vkCmdEndRenderPass( commandBuffer );
            }

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
    void Renderer::SubmitCommandBuffers()
    {
        std::vector<VkPipelineStageFlags> waitSemaphoreStages = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo;
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext                = nullptr;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = mWindow.mSwapchain.GetCurrentImageAvailableSemaphore();
        submitInfo.pWaitDstStageMask    = waitSemaphoreStages.data();
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &mPrimaryCommandBuffers.mBuffers[mWindow.mSwapchain.mCurrentImageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = mWindow.mSwapchain.GetCurrentRenderFinishedSemaphore();

        VkResult result = vkQueueSubmit( mDevice.mGraphicsQueue,
                                         1,
                                         &submitInfo,
                                         *mWindow.mSwapchain.GetCurrentInFlightFence() );
        if( result != VK_SUCCESS )
        {
            Debug::Error( "Could not submit draw command buffer " );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::ReloadIcons()
    {
        WaitIdle();
        mDrawImgui.ReloadIcons( mDevice );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::CreateCommandBuffers()
    {
        const uint32_t count = mWindow.mSwapchain.mImagesCount;
        mPrimaryCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_PRIMARY );

        mDrawModels.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
        mDrawImgui.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
        mDrawUI.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
        mDrawPostprocess.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
        mDrawDebug.mCommandBuffers.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
        mDrawDebug.mCommandBuffers2D.Create( mDevice, count, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::CreateRenderPasses()
    {
        // game
        {
            VkAttachmentDescription colorAtt           = RenderPass::GetColorAttachment(
                    mWindow.mSwapchain.mSurfaceFormat.format,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
            VkAttachmentReference   colorAttRef        = RenderPass::GetColorAttachmentReference( 0 );
            VkAttachmentDescription depthAtt           = RenderPass::GetDepthAttachment( mDevice.FindDepthFormat() );
            VkAttachmentReference   depthAttRef        = RenderPass::GetDepthAttachmentReference( 1 );
            VkSubpassDescription    subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef,
                                                                                            1,
                                                                                            &depthAttRef );
            VkSubpassDependency     subpassDependency  = RenderPass::GetDependency();

            VkAttachmentDescription attachmentDescriptions[2] = { colorAtt, depthAtt };
            mRenderPassGame.Create( mDevice,
                                    attachmentDescriptions,
                                    2,
                                    &subpassDescription,
                                    1,
                                    &subpassDependency,
                                    1 );
        }

        // postprocess
        {
            const VkImageLayout     ppLayout           = ( mViewType == ViewType::Editor
                    ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
            VkAttachmentDescription colorAtt           = RenderPass::GetColorAttachment(
                    mWindow.mSwapchain.mSurfaceFormat.format,
                    ppLayout );
            VkAttachmentReference   colorAttRef        = RenderPass::GetColorAttachmentReference( 0 );
            VkSubpassDescription    subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef,
                                                                                            1,
                                                                                            VK_NULL_HANDLE );
            VkSubpassDependency     subpassDependency  = RenderPass::GetDependency();
            mRenderPassPostprocess.Create( mDevice,
                                           &colorAtt,
                                           1,
                                           &subpassDescription,
                                           1,
                                           &subpassDependency,
                                           1 );
        }

        // imgui
        if( mViewType == ViewType::Editor )
        {
            VkAttachmentDescription colorAtt           = RenderPass::GetColorAttachment(
                    mWindow.mSwapchain.mSurfaceFormat.format,
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );
            VkAttachmentReference   colorAttRef        = RenderPass::GetColorAttachmentReference( 0 );
            VkSubpassDescription    subpassDescription = RenderPass::GetSubpassDescription( &colorAttRef,
                                                                                            1,
                                                                                            VK_NULL_HANDLE );
            VkSubpassDependency     subpassDependency  = RenderPass::GetDependency();
            mRenderPassImgui.Create( mDevice, &colorAtt, 1, &subpassDescription, 1, &subpassDependency, 1 );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::CreateFramebuffers( const VkExtent2D _extent )
    {
        const VkFormat depthFormat  = mDevice.FindDepthFormat();
        const VkFormat colorFormat  = mWindow.mSwapchain.mSurfaceFormat.format;
        const uint32_t imagesCount  = mWindow.mSwapchain.mImagesCount;

        // game color
        mImageGameColor.Create( mDevice, colorFormat, _extent,
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                VK_IMAGE_USAGE_STORAGE_BIT |
                                VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                VK_IMAGE_USAGE_SAMPLED_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        mImageViewGameColor.Create( mDevice,
                                    mImageGameColor.mImage,
                                    colorFormat,
                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                    VK_IMAGE_VIEW_TYPE_2D );
        // game depth
        mImageGameDepth.Create( mDevice,
                                depthFormat,
                                _extent,
                                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
        mImageViewGameDepth.Create( mDevice,
                                    mImageGameDepth.mImage,
                                    depthFormat,
                                    VK_IMAGE_ASPECT_DEPTH_BIT,
                                    VK_IMAGE_VIEW_TYPE_2D );
        {
            VkCommandBuffer cmd = mDevice.BeginSingleTimeCommands();
            mImageGameDepth.TransitionImageLayout( cmd,
                                                   depthFormat,
                                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                                   1 );
            mDevice.EndSingleTimeCommands( cmd );
        }
        VkImageView    gameViews[2] = { mImageViewGameColor.mImageView, mImageViewGameDepth.mImageView };
        mFrameBuffersGame.Create( mDevice, imagesCount, _extent, mRenderPassGame, gameViews, 2 );

        // pp color
        mImagePostprocessColor.Create( mDevice, colorFormat, _extent,
                                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                       VK_IMAGE_USAGE_STORAGE_BIT |
                                       VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                       VK_IMAGE_USAGE_SAMPLED_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        mImageViewPostprocessColor.Create( mDevice,
                                           mImagePostprocessColor.mImage,
                                           colorFormat,
                                           VK_IMAGE_ASPECT_COLOR_BIT,
                                           VK_IMAGE_VIEW_TYPE_2D );
        mFramebuffersPostprocess.Create( mDevice,
                                         imagesCount,
                                         _extent,
                                         mRenderPassPostprocess,
                                         &mImageViewPostprocessColor.mImageView,
                                         1 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Renderer::CreateTextureDescriptor()
    {
        const std::vector<Texture*>& textures = mTextureManager.GetTextures();
        std::vector<VkImageView> imageViews( textures.size() );
        for( int                 i            = 0; i < (int)textures.size(); i++ )
        {
            imageViews[i] = textures[i]->mImageView;
        }
        mDescriptorTextures.Destroy( mDevice );
        mDescriptorTextures.Create( mDevice, imageViews.data(), static_cast<uint32_t>( imageViews.size() ) );
    }
}