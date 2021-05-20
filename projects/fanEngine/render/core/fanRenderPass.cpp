#include "render/core/fanRenderPass.hpp"

#include "render/core/fanDevice.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool RenderPass::Create( Device& _device,
                             VkAttachmentDescription* _attachmentdescriptions,
                             uint32_t _countAttachments,
                             VkSubpassDescription* _subpassDescriptions,
                             uint32_t _countSubpasses,
                             VkSubpassDependency* _dependencies,
                             uint32_t _countDependencies )
    {
        fanAssert( mRenderPass == VK_NULL_HANDLE );

        VkRenderPassCreateInfo renderPassCreateInfo;
        renderPassCreateInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.pNext           = nullptr;
        renderPassCreateInfo.flags           = 0;
        renderPassCreateInfo.attachmentCount = _countAttachments;
        renderPassCreateInfo.pAttachments    = _attachmentdescriptions;
        renderPassCreateInfo.subpassCount    = _countSubpasses;
        renderPassCreateInfo.pSubpasses      = _subpassDescriptions;
        renderPassCreateInfo.dependencyCount = _countDependencies;
        renderPassCreateInfo.pDependencies   = _dependencies;

        if( vkCreateRenderPass( _device.mDevice, &renderPassCreateInfo, nullptr, &mRenderPass ) != VK_SUCCESS )
        {
            Debug::Error( "Could not create render pass" );
            return false;
        }
        Debug::Log() << Debug::Type::Render << std::hex << "VkRenderPass          " << mRenderPass << std::dec << Debug::Endl();

        _device.AddDebugName( (uint64_t)mRenderPass, "RenderPass" );

        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderPass::Destroy( Device& _device )
    {
        if( mRenderPass != VK_NULL_HANDLE )
        {
            vkDestroyRenderPass( _device.mDevice, mRenderPass, VK_NULL_HANDLE );
            _device.RemoveDebugName( (uint64_t)mRenderPass );
            mRenderPass = VK_NULL_HANDLE;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkAttachmentReference RenderPass::GetColorAttachmentReference( const uint32_t _index )
    {
        VkAttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = _index;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        return colorAttachmentRef;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkAttachmentDescription RenderPass::GetColorAttachment( const VkFormat _format,
                                                            const VkImageLayout _finalLayout )
    {
        VkAttachmentDescription colorAttachment;
        colorAttachment.flags          = 0;
        colorAttachment.format         = _format;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = _finalLayout;

        return colorAttachment;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkAttachmentReference RenderPass::GetDepthAttachmentReference( const uint32_t _index )
    {
        VkAttachmentReference depthAttachmentRef;
        depthAttachmentRef.attachment = _index;
        depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        return depthAttachmentRef;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkAttachmentDescription RenderPass::GetDepthAttachment( const VkFormat _format )
    {
        VkAttachmentDescription depthAttachment;
        depthAttachment.flags          = 0;
        depthAttachment.format         = _format;
        depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        return depthAttachment;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkSubpassDependency RenderPass::GetDependency()
    {
        VkSubpassDependency dependency;
        dependency.srcSubpass      = 0;
        dependency.dstSubpass      = VK_SUBPASS_EXTERNAL;
        dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependency.srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
        dependency.dependencyFlags = 0;

        return dependency;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkSubpassDescription RenderPass::GetSubpassDescription( VkAttachmentReference* _colorReferences,
                                                            uint32_t _count,
                                                            VkAttachmentReference* _depthReference )
    {
        VkSubpassDescription subpassDescription;
        subpassDescription.flags                   = 0;
        subpassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.inputAttachmentCount    = 0;
        subpassDescription.pInputAttachments       = VK_NULL_HANDLE;
        subpassDescription.colorAttachmentCount    = _count;
        subpassDescription.pColorAttachments       = _colorReferences;
        subpassDescription.pResolveAttachments     = VK_NULL_HANDLE;
        subpassDescription.pDepthStencilAttachment = _depthReference;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments    = VK_NULL_HANDLE;

        return subpassDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkRenderPassBeginInfo RenderPass::GetBeginInfo( VkRenderPass _renderPass,
                                                    VkFramebuffer _frameBuffer,
                                                    VkExtent2D _extent,
                                                    const VkClearValue* _clearValue,
                                                    uint32_t _clearCount )
    {
        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext                    = nullptr;
        beginInfo.renderPass               = _renderPass;
        beginInfo.framebuffer              = _frameBuffer;
        beginInfo.renderArea.offset        = { 0, 0 };
        beginInfo.renderArea.extent.width  = _extent.width;
        beginInfo.renderArea.extent.height = _extent.height;
        beginInfo.clearValueCount          = _clearCount;
        beginInfo.pClearValues             = _clearValue;

        return beginInfo;
    }
}