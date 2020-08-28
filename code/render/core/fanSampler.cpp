#include "render/core/fanSampler.hpp"
#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Sampler::Destroy( Device& _device )
	{
		if( mSampler != VK_NULL_HANDLE )
		{
			vkDestroySampler( _device.mDevice, mSampler, VK_NULL_HANDLE );
			_device.RemoveDebugName( (uint64_t)mSampler );
			mSampler = VK_NULL_HANDLE;
		}
	}

	//========================================================================================================
	//========================================================================================================
    void Sampler::Create( Device& _device,
                          const float _maxLod,
                          const float _maxAnisotropy,
                          const VkFilter _filter )
	{
        fanAssert( mSampler == VK_NULL_HANDLE );

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = _filter;
		samplerInfo.minFilter = _filter;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = _maxAnisotropy > 0.f ? VK_TRUE : VK_FALSE;
		samplerInfo.maxAnisotropy = _maxAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		samplerInfo.minLod = 0;//to test

		samplerInfo.maxLod = _maxLod;
		samplerInfo.mipLodBias = 0;

		if( vkCreateSampler( _device.mDevice, &samplerInfo, VK_NULL_HANDLE, &mSampler ) != VK_SUCCESS )
		{
			Debug::Error() << "sampler creation failed" << Debug::Endl();
		}
		else
		{
			_device.AddDebugName( (uint64_t)mSampler, "Sampler" );
		}
	}
}