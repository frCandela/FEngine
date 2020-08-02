#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "glfw/glfw3.h"
#include "core/resources/fanResource.hpp"
#include "render/resources/fanTextureManager.hpp"

namespace fan
{
	struct Device;

	//========================================================================================================
	// like an Image but generated from a png file
	//========================================================================================================
	struct Texture : public Resource
	{
		static TextureManager s_resourceManager;		

		VkImage			mImage		= VK_NULL_HANDLE;
		VkImageView		mImageView	= VK_NULL_HANDLE;
		VkDeviceMemory	mMemory		= VK_NULL_HANDLE;

		uint32_t	    mMipLevels = 1;
		VkExtent2D	    mExtent;
		uint32_t	    mLayerCount = 1;
		std::string	    mPath;
        uint8_t *       mPixels = nullptr;
		int             mIndex = -1;
        bool            mBuffersOutdated = false;

        void LoadFromPixels(
                const uint8_t* _pixelsRGBA32,
                const VkExtent2D _extent,
                const uint32_t _mipLevels );
		bool LoadFromFile( const std::string& _path );
		void Create( Device& _device );
		void Destroy( Device& _device );
		void FreePixels();

	private:
        void CreateImage( Device& _device,
                          VkExtent2D _extent,
                          uint32_t _mipLevels,
                          VkFormat _format,
                          VkImageTiling _tiling,
                          VkImageUsageFlags _usage,
                          VkMemoryPropertyFlags _properties );

        void CreateImageView( Device& _device,
                              VkFormat _format,
                              VkImageViewType _viewType,
                              VkImageSubresourceRange _subresourceRange );

        void CopyBufferToImage( VkCommandBuffer _commandBuffer, VkBuffer _buffer, VkExtent2D _extent );

        void GenerateMipmaps( Device& _device,
                              VkCommandBuffer _commandBuffer,
                              VkFormat _imageFormat,
                              VkExtent2D _extent,
                              uint32_t _mipLevels );

        void TransitionImageLayout( VkCommandBuffer _commandBuffer,
                                    VkImageLayout _oldLayout,
                                    VkImageLayout _newLayout,
                                    VkImageSubresourceRange _subresourceRange );
    };
}