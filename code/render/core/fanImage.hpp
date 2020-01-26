#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan
{
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class Image {
	public:
		Image(Device & _device);
		~Image();

		bool Create( const VkFormat _format, const VkExtent2D _size, const VkImageUsageFlags _usage, const VkMemoryPropertyFlags _memoryProperties);
		void TransitionImageLayout(VkCommandBuffer _commandBuffer, VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, uint32_t _mipLevels);

		VkImage		GetImage() { return m_image; }
		VkExtent2D  GetSize() const { return m_size; }
		void		Resize( const VkExtent2D _size );

	private:
		Device &		m_device;

		VkMemoryPropertyFlags	m_memoryProperties;
		VkImageUsageFlags		m_usage;
		VkExtent2D				m_size;
		VkFormat				m_format;

		VkImage			m_image;
		VkDeviceMemory	m_imageMemory;

		void DestroyImage();
		bool HasStencilComponent(VkFormat _format) { return _format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT; }
	};
}