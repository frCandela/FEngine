#pragma once

#include "render/fanRenderPrecompiled.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
	class Device;
	class Buffer;

	//================================================================================================================================
	//================================================================================================================================
	class Texture : public Resource< Texture >
	{
	public:
		static Signal< Texture* > s_onGenerateVulkanData;
		static Signal< Texture* > s_onDeleteVulkanData;

		~Texture();

		void SetData(const unsigned char * _data, const uint32_t _width, const uint32_t _height, const uint32_t _mipLevels);
		bool LoadFromFile( const std::string& _path );
		void GenerateVulkanData( Device & _device );
		void DeleteVulkanData( Device & _device );

		std::string GetPath() const { return m_path; }
		glm::ivec3	GetSize() const { return glm::ivec3(m_width, m_height, m_layerCount); }
		VkImageView GetImageView()  { return m_imageView; }

		int		GetRenderID() const { return m_renderID; }
		void	SetRenderID(const int _renderID) { m_renderID = _renderID; }

	private:
		VkImage			m_image			= VK_NULL_HANDLE;
		VkDeviceMemory	m_deviceMemory	= VK_NULL_HANDLE;
		VkImageView		m_imageView		= VK_NULL_HANDLE;

		uint32_t m_mipLevels = 1;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_layerCount = 1;

		unsigned char *	 m_data;
		std::string		 m_path;

		int m_renderID = -1;
		
		void CreateImage( Device & _device, VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
		void CreateImageView( Device & _device, VkFormat _format, VkImageViewType _viewType, VkImageSubresourceRange _subresourceRange);

		void CopyBufferToImage(VkCommandBuffer _commandBuffer, VkBuffer _buffer, uint32_t _width, uint32_t _height);
		void GenerateMipmaps( Device & _device,VkCommandBuffer _commandBuffer, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels);
		void TransitionImageLayout( VkCommandBuffer _commandBuffer, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkImageSubresourceRange _subresourceRange);

	};
}