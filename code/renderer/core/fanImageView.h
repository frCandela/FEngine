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

		bool Create(VkImage _image, const VkFormat _format, const VkImageAspectFlags _aspectFlags, const VkImageViewType _viewType);
		void SetImage( VkImage _image );

		VkImageView GetImageView() { return m_imageView; }
	private:
		Device &	m_device;

		VkImageView			m_imageView;
		VkFormat			m_format; 
		VkImageAspectFlags	m_aspectFlags; 
		VkImageViewType		m_viewType;

		void Destroy();
	};
}