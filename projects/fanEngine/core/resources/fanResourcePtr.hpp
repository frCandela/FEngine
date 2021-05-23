#pragma once

#include "core/fanHash.hpp"
#include "core/ecs/fanSignal.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
    class ResourceManager;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ResourcePtrData
    {
        uint32_t mGUID = 0;
        ResourceHandle * mHandle = nullptr; // owned by the resource manager
        static ResourceManager* sResourceManager;
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
            if( mData.mHandle != nullptr && mData.mHandle->mResource != nullptr )
            {
                return static_cast<ResourceType*>( mData.mHandle->mResource);
            }
            return nullptr;
        }
        ResourcePtr( ResourceHandle* _handle = nullptr )
        {
            mData.mHandle          = _handle;
            mData.mGUID            = _handle != nullptr && _handle->mResource != nullptr ? _handle->mResource->mGUID : 0;
        }
        ResourcePtrData mData;
    };
}