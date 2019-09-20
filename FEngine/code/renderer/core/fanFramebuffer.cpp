#include "fanGlobalIncludes.h"

#include "renderer/core/fanFrameBuffer.h"
#include "renderer/core/fanDevice.h"

namespace fan
{
	namespace vk {

		//================================================================================================================================
		//================================================================================================================================
		FrameBuffer::FrameBuffer(Device & _device) :
			m_device(_device) {
		}

		//================================================================================================================================
		//================================================================================================================================
		FrameBuffer::~FrameBuffer() {
			if (m_framebuffer != VK_NULL_HANDLE) {
				vkDestroyFramebuffer(m_device.vkDevice, m_framebuffer, nullptr);
				m_framebuffer = VK_NULL_HANDLE;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		bool FrameBuffer::Create(VkRenderPass _renderPass, std::vector<VkImageView> & _attachments, VkExtent2D _size) {
			VkFramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = _renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(_attachments.size());
			framebufferCreateInfo.pAttachments = _attachments.data();
			framebufferCreateInfo.width = _size.width;
			framebufferCreateInfo.height = _size.height;
			framebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(m_device.vkDevice, &framebufferCreateInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
				fan::Debug::Error("Could not create framebuffer");
				return false;
			}
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkFramebuffer         " << m_framebuffer << std::dec << Debug::Endl();
			return true;
		}
	}
}