#pragma once

namespace vk {

	class Device;

	class FrameBuffer {
	public:
		FrameBuffer(Device * _device);
		~FrameBuffer();

		bool Create(VkRenderPass _renderPass, std::vector<VkImageView> & _attachments, VkExtent2D _size);

		VkFramebuffer GetFrameBuffer() { return m_framebuffer; }

	private:
		Device * m_device;
		VkFramebuffer m_framebuffer;
	};
}