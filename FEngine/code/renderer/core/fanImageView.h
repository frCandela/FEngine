#pragma once

namespace fan
{
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class ImageView {
	public:
		ImageView(Device & _device);
		ImageView(const ImageView&) = delete;
		~ImageView();

		bool Create(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags, VkImageViewType _viewType);

		VkImageView GetImageView() { return m_imageView; }
	private:
		Device &	m_device;
		VkImageView m_imageView;
	};
}