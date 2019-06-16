#pragma once

#include "AllInclude.h"

#include "vkDevice.h"

namespace vk {
	class FrameBuffer {
	public:
		FrameBuffer(Device * _device) :
			m_device(_device) {
		}

		~FrameBuffer() {
			if (m_framebuffer != VK_NULL_HANDLE) {
				vkDestroyFramebuffer(m_device->vkDevice, m_framebuffer, nullptr);
				m_framebuffer = VK_NULL_HANDLE;
			}
		}

		bool Create(VkRenderPass _renderPass, std::vector<VkImageView> & _attachments, VkExtent2D _size) {
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

			if ( vkCreateFramebuffer(m_device->vkDevice, &framebufferCreateInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
				std::cout << "Could not create shader module" << std::endl;
				return false;
			}
			std::cout << std::hex << "VkFramebuffer\t\t" << m_framebuffer << std::dec << std::endl;
			return true;
		}

		VkFramebuffer GetFrameBuffer() { return m_framebuffer; }
	private:
		Device * m_device;
		VkFramebuffer m_framebuffer;

	};
}