#pragma once

#include "core/fanHash.hpp"
#include "core/fanSignal.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
    class Resources;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ResourcePtrData
    {
        uint32_t mGUID = 0;
        ResourceHandle  * mHandle = nullptr; // owned by the resource manager
        static Resources* sResourceManager;

        Resource* GetResource() const { return mHandle != nullptr && mHandle->mResource != nullptr ? mHandle->mResource : nullptr; }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename ResourceType >
    struct ResourcePtr
    {
        ResourceType* operator->() const
        {
            ResourceType* resource = *this;
            return resource;
        }
        ResourceType& operator*() const
        {
            ResourceType* resource = *this;
            return *resource;
        }
        operator ResourceType*() const
        {
            return static_cast<ResourceType*>(mData.GetResource());
        }
        ResourcePtr( ResourceHandle* _handle = nullptr )
        {
            mData.mHandle = _handle;
            mData.mGUID   = _handle != nullptr && _handle->mResource != nullptr ? _handle->mResource->mGUID : 0;
        }
        operator const ResourcePtrData&() const { return mData; }
        operator ResourcePtrData&() { return mData; }

        ResourcePtrData mData;
    };
}