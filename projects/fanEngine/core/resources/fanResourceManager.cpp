#include "core/resources/fanResourceManager.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* ResourceManager::Load( const uint32_t _type, const std::string& _path )
    {
        ResourceInfo& info = mResourceInfos[_type];
        fanAssert( info.mLoad != nullptr );
        Resource* resource = ( *info.mLoad )( _path, info );
        if( resource != nullptr )
        {
            Add( _type, resource, _path );
        }
        return resource;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ResourceManager::Add( const uint32_t _type, Resource* _resource, const std::string& _path )
    {
        _resource->mGUID = DSID( _path.c_str() );
        _resource->mType = _type;
        mResources[_resource->mGUID] = _resource;
        ResourceInfo& info = mResourceInfos[_type];
        info.mCount++;

        if( info.mUseDirtyList )
        {
            info.mDirtyList.push_back( _resource );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* ResourceManager::Get( const uint32_t _type, const uint32_t _guid ) const
    {
        auto it = mResources.find( _guid );
        if( it != mResources.end() )
        {
            Resource* resource = it->second;
            fanAssert( resource->mType == _type );
            (void)_type;
            return resource;
        }
        return nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* ResourceManager::GetOrLoad( const uint32_t _type, const std::string& _path )
    {
        Resource* resource = Get( _type, DSID( _path.c_str() ) );
        if( resource == nullptr )
        {
            return Load( _type, _path );
        }
        return resource;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool ResourceManager::Remove( const uint32_t _guid )
    {
        auto it = mResources.find( _guid );
        if( it != mResources.end() )
        {
            DeleteResource( it );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ResourceManager::DeleteResource( std::map<uint32_t, Resource*>::iterator _iterator )
    {
        Resource* resource = _iterator->second;
        mResources.erase( _iterator );
        ResourceInfo& info = mResourceInfos[resource->mType];
        info.mCount--;

        if( info.mUseDestroyList )
        {
            info.mDestroyList.push_back( resource );
        }
        else
        {
            delete resource;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool ResourceManager::SetOutdated( const uint32_t _guid )
    {
        auto it = mResources.find( _guid );
        if( it != mResources.end() )
        {
            Resource    * resource = it->second;
            ResourceInfo& info     = mResourceInfos[resource->mType];
            info.mDirtyList.push_back( resource );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ResourceManager::ResolvePtr( ResourcePtrData& _resourcePtrData )
    {
        _resourcePtrData.mResource = GetOrLoad( _resourcePtrData.mType, _resourcePtrData.mPath );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ResourceManager::Clear()
    {
        while( !mResources.empty() )
        {
            DeleteResource( mResources.begin() );
        }

        for( auto& pair : mResourceInfos )
        {
            ResourceInfo& resourceInfo = pair.second;
            fanAssert( resourceInfo.mCount == 0 );
            (void)resourceInfo;
        }
    }
}