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
            resource->mGUID = DSID( _path.c_str() );
            resource->mType = _type;
            fanAssert( mResources.find( resource->mGUID ) == mResources.end() );
            mResources[resource->mGUID] = resource;
            info.mCount++;
        }
        return resource;
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
    void ResourceManager::ResolvePtr( ResourcePtrData& _resourcePtrData )
    {
        _resourcePtrData.mResource = GetOrLoad( _resourcePtrData.mType, _resourcePtrData.mPath );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ResourceManager::Clear()
    {
        for( auto pair : mResources )
        {
            delete pair.second;
        }
        mResources.clear();

        for( auto& pair : mResourceInfos )
        {
            pair.second.mCount = 0;
        }
    }
}