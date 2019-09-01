#pragma once

namespace fan
{
	namespace vk {

		class Device;

		//================================================================================================================================
		//================================================================================================================================
		class Image {
		public:
			Image(Device & _device);
			~Image();

			bool Create(VkFormat _format, VkExtent2D _size, VkImageUsageFlags _usage, VkMemoryPropertyFlags _memoryProperties);
			void TransitionImageLayout(VkCommandBuffer _commandBuffer, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _mipLevels);

			VkImage GetImage() { return m_image; }
		private:
			Device &		m_device;

			VkImage			m_image;
			VkDeviceMemory	m_imageMemory;

			bool HasStencilComponent(VkFormat _format) { return _format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT; }
		};
	}
}