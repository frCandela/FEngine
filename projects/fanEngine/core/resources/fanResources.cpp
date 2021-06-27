#include "core/resources/fanResources.hpp"
#include "core/fanDebug.hpp"
#include "core/fanPath.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourcePtrData Resources::Load( const uint32_t _type, const std::string& _path )
    {
        ResourcePtrData resourcePtrData;
        resourcePtrData.mHandle = LoadInternal( _type, _path );
        resourcePtrData.mGUID   = resourcePtrData.mHandle != nullptr ? resourcePtrData.mHandle->mResource->mGUID : 0;
        return resourcePtrData;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourcePtrData Resources::Get( const uint32_t _guid )
    {
        ResourcePtrData resourcePtrData;
        resourcePtrData.mHandle = GetInternal( _guid );
        resourcePtrData.mGUID   = _guid;
        return resourcePtrData;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourcePtrData Resources::GetOrLoad( const uint32_t _type, const std::string& _path )
    {
        fanAssert( !Path::IsAbsolute( _path ) );
        ResourcePtrData resourcePtrData;
        resourcePtrData.mGUID   = DSID( _path.c_str() );
        resourcePtrData.mHandle = GetOrLoadInternal( _type, _path );
        return resourcePtrData;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourceHandle* Resources::LoadInternal( const uint32_t _type, const std::string& _path )
    {
        auto infoIt = mResourceInfos.find( _type );
        if( infoIt == mResourceInfos.end() )
        {
            Debug::Error() << Debug::Type::Engine << "failed to load resource with unknown type ID " << _type << Debug::Endl();
            return nullptr;
        }

        ResourceInfo& info = infoIt->second;
        fanAssert( info.mLoad != nullptr );
        Resource* resource = ( *info.mLoad )( _path, info );
        if( resource != nullptr )
        {
            return AddInternal( _type, resource, _path );
        }
        return nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourceHandle* Resources::AddInternal( const uint32_t _type, Resource* _resource, const std::string& _path )
    {
        fanAssert( !Path::IsAbsolute( _path ) );
        _resource->mGUID = DSID( _path.c_str() );

        // Create resource handle
        ResourceHandle* resourceHandle = nullptr;
        auto it = mResourceHandles.find( _resource->mGUID );
        if( it != mResourceHandles.end() )
        {
            resourceHandle = it->second;
            if( resourceHandle->mResource != nullptr )
            {
                Debug::Log() << Debug::Type::Engine << "reloading resource " << _path << Debug::Endl();
                fanAssert( !resourceHandle->mResource->mIsDirty );
                DeleteResource( it );
            }
        }
        else
        {
            resourceHandle = new ResourceHandle();
            mResourceHandles[_resource->mGUID] = resourceHandle;
        }

        mResources[_resource->mGUID] = _resource;

        ResourceInfo& info = mResourceInfos.at( _type );
        if( info.mUseDirtyList )
        {
            _resource->mIsDirty = true;
            info.mDirtyList.push_back( _resource );
        }

        info.mCount++;
        _resource->mPath          = _path;
        _resource->mType          = _type;
        resourceHandle->mResource = _resource;
        resourceHandle->mRefCount++;
        return resourceHandle;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourceHandle* Resources::GetInternal( const uint32_t _guid )
    {
        auto it = mResourceHandles.find( _guid );
        if( it != mResourceHandles.end() )
        {
            ResourceHandle* resource = it->second;
            return resource;
        }
        return nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ResourceHandle* Resources::GetOrLoadInternal( const uint32_t _type, const std::string& _path )
    {
        fanAssert( !Path::IsAbsolute( _path ) );
        ResourceHandle* handle = GetInternal( DSID( _path.c_str() ) );
        if( handle == nullptr )
        {
            return LoadInternal( _type, _path );
        }
        return handle;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Resources::Remove( const uint32_t _guid )
    {
        auto it = mResourceHandles.find( _guid );
        if( it != mResourceHandles.end() )
        {
            return DeleteResource( it );
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Resources::DeleteResource( std::map<uint32_t, ResourceHandle*>::iterator _iterator )
    {
        const uint32_t guid = _iterator->first;
        auto           it   = mResources.find( guid );
        Resource* resource = nullptr;
        if( it != mResources.end() )
        {
            resource = it->second;
            mResources.erase( it );
        }

        if( resource != nullptr )
        {
            ResourceInfo& info = mResourceInfos.at( resource->mType );
            info.mCount--;

            if( info.mUseDestroyList )
            {
                info.mDestroyList.push_back( resource );
            }
            else
            {
                delete resource;
            }
            ResourceHandle* handle = _iterator->second;
            handle->mResource = nullptr;
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Resources::SetDirty( const uint32_t _guid )
    {
        auto it = mResourceHandles.find( _guid );
        if( it != mResourceHandles.end() )
        {
            ResourceHandle* handle = it->second;
            if( handle->mResource == nullptr )
            {
                return false;
            }

            if( !handle->mResource->mIsDirty )
            {
                ResourceInfo& info = mResourceInfos.at( handle->mResource->mType );
                fanAssert( info.mUseDirtyList );
                handle->mResource->mIsDirty = true;
                info.mDirtyList.push_back( handle->mResource );
                return true;
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Resources::Clear()
    {
        for( auto it = mResourceHandles.begin(); it != mResourceHandles.end(); it++ )
        {
            DeleteResource( it );
        }

        for( auto& pair : mResourceInfos )
        {
            ResourceInfo& resourceInfo = pair.second;
            fanAssert( resourceInfo.mCount == 0 ); // Some resources have not been destroyed properly
            (void)resourceInfo;
        }
    }
}