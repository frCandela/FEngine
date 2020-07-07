#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "glfw/glfw3.h"
#include "core/resources/fanResource.hpp"
#include "render/fanTextureManager.hpp"

namespace fan
{
	class Device;

	//================================================================================================================================
	// like an Image but generated from a png file
	//================================================================================================================================
	struct Texture : public Resource
	{
		static TextureManager s_resourceManager;		

		VkImage			image = VK_NULL_HANDLE;
		VkImageView		imageView = VK_NULL_HANDLE;
		VkDeviceMemory	deviceMemory = VK_NULL_HANDLE;

		uint32_t	mipLevels = 1;
		VkExtent2D	extent;
		uint32_t	layerCount = 1;

		std::string	   path;
		int renderID = -1;

		bool CreateFromFile( const std::string& _path );
		void CreateFromData( const unsigned char* _data, const VkExtent2D _extent, const uint32_t _mipLevels );
		void Destroy( Device& _device );

	private:
		void GenerateGpuData( Device& _device, const void* _data );
		void DeleteGpuData( Device& _device );
		void CreateImage( Device& _device, VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties );
		void CreateImageView( Device& _device, VkFormat _format, VkImageViewType _viewType, VkImageSubresourceRange _subresourceRange );
		void CopyBufferToImage( VkCommandBuffer _commandBuffer, VkBuffer _buffer, VkExtent2D _extent );
		void GenerateMipmaps( Device& _device, VkCommandBuffer _commandBuffer, VkFormat _imageFormat, VkExtent2D _extent, uint32_t _mipLevels );
		void TransitionImageLayout( VkCommandBuffer _commandBuffer, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkImageSubresourceRange _subresourceRange );
	};
}