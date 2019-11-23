#pragma once

namespace fan
{
	class Device;
	class Buffer;

	//================================================================================================================================
	//================================================================================================================================
	class Texture
	{
	public:
		Texture(Device & _device);
		~Texture();

		void Load(const void * _data, const uint32_t _width, const uint32_t _height, const uint32_t _mipLevels);
		bool LoadTexture(const std::string _path);

		glm::ivec3	GetSize() const { return glm::ivec3(m_width, m_height, m_layerCount); }
		std::string GetPath() const { return m_path; }
		VkImageView GetImageView() { return m_imageView; }

		int		GetRenderID() const { return m_renderID; }
		void	SetRenderID(const int _renderID) { m_renderID = _renderID; }

	private:
		Device &		m_device;
		VkImage			m_image;
		VkDeviceMemory	m_deviceMemory;
		VkImageView		m_imageView;

		std::string		m_path;
		uint32_t		m_mipLevels = 1;
		uint32_t		m_width;
		uint32_t		m_height;
		uint32_t		m_layerCount = 1;

		int m_renderID = -1;

		void Destroy();
		void CreateImage(VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
		void CreateImageView(VkFormat _format, VkImageViewType _viewType, VkImageSubresourceRange _subresourceRange);

		void CopyBufferToImage(VkCommandBuffer _commandBuffer, VkBuffer _buffer, uint32_t _width, uint32_t _height);
		void GenerateMipmaps(VkCommandBuffer _commandBuffer, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels);
		void TransitionImageLayout(VkCommandBuffer _commandBuffer, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkImageSubresourceRange _subresourceRange);

	};
}