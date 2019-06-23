#pragma once

namespace vk {

	class Device;

	class ImageView {
	public:
		ImageView(Device * _device);

		~ImageView();

		bool Create(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags);

		VkImageView GetImageView() { return m_imageView; }
	private:
		Device *	m_device;
		VkImageView m_imageView;
	};
}