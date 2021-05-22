#pragma once

#include <vector>
#include "core/fanHash.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ResourceInfo
    {
        Resource* (* mLoad)( const std::string& _path, ResourceInfo& _info ) = nullptr;

        bool mUseDestroyList = false;
        bool mUseDirtyList   = false;
        void* mDataPtr = nullptr;
        int mCount = 0;

        std::vector<Resource*> mDestroyList;
        std::vector<Resource*> mDirtyList;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class ResourceManager
    {
    public:
        template< class ResourceType >
        void AddResourceType( const ResourceInfo& _resourceInfo );

        template< class ResourceType >
        ResourceType* Load( const std::string& _path );

        template< class ResourceType >
        void Add( ResourceType* _resource, const std::string& _path );

        template< class ResourceType >
        ResourceType* Get( const std::string& _path ) const;

        template< class ResourceType >
        ResourceType* Get( const uint32_t _guid ) const;

        template< class ResourceType >
        void Get( std::vector<ResourceType*>& _outResources ) const;

        template< class ResourceType >
        ResourceType* GetOrLoad( const std::string& _path );

        bool Remove( const std::string& _path ) { return Remove( DSID( _path.c_str() ) ); }
        bool Remove( const uint32_t _guid );

        bool SetOutdated( const uint32_t _guid );

        template< class ResourceType >
        void GetDestroyList( std::vector<ResourceType*>& _destroyedList );

        template< class ResourceType >
        void GetDirtyList( std::vector<ResourceType*>& _outdatedList );

        template< class ResourceType >
        int Count() const;

        void ResolvePtr( ResourcePtrData& _resourcePtrData );

        void Clear();

    private:
        Resource* Load( const uint32_t _type, const std::string& _path );
        void Add( const uint32_t _type, Resource* _resource, const std::string& _path );
        Resource* Get( const uint32_t _type, const uint32_t _guid ) const;
        Resource* GetOrLoad( const uint32_t _type, const std::string& _path );
        void DeleteResource( std::map<uint32_t, Resource*>::iterator _iterator );

        std::map<uint32_t, Resource*>    mResources;
        std::map<uint32_t, ResourceInfo> mResourceInfos;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void ResourceManager::AddResourceType( const ResourceInfo& _resourceInfo )
    {
        fanAssert( mResourceInfos.find( ResourceType::Info::sType ) == mResourceInfos.end() );
        ResourceInfo resourceInfo = _resourceInfo;
        resourceInfo.mCount = 0;
        mResourceInfos[ResourceType::Info::sType] = resourceInfo;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourceType* ResourceManager::Load( const std::string& _path )
    {
        return static_cast<ResourceType*>(Load( ResourceType::Info::sType, _path ));
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void ResourceManager::Add( ResourceType* _resource, const std::string& _path )
    {
        Add( ResourceType::Info::sType, _resource, _path );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourceType* ResourceManager::Get( const std::string& _path ) const
    {
        return Get<ResourceType>( DSID( _path.c_str() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourceType* ResourceManager::Get( const uint32_t _guid ) const
    {
        return static_cast<ResourceType*>( Get( ResourceType::Info::sType, _guid ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourceType* ResourceManager::GetOrLoad( const std::string& _path )
    {
        return static_cast<ResourceType*>( GetOrLoad( ResourceType::Info::sType, _path ));
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void ResourceManager::GetDestroyList( std::vector<ResourceType*>& _destroyedList )
    {
        ResourceInfo& info = mResourceInfos[ResourceType::Info::sType];
        fanAssert( info.mUseDestroyList );
        _destroyedList.clear();
        for( Resource* resource : info.mDestroyList )
        {
            _destroyedList.push_back( static_cast<ResourceType*>(resource) );
        }
        info.mDestroyList.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void ResourceManager::GetDirtyList( std::vector<ResourceType*>& _outdatedList )
    {
        ResourceInfo& info = mResourceInfos[ResourceType::Info::sType];
        fanAssert( info.mUseDirtyList );
        _outdatedList.clear();
        for( Resource* resource : info.mDirtyList )
        {
            _outdatedList.push_back( static_cast<ResourceType*>(resource) );
        }
        info.mDirtyList.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    int ResourceManager::Count() const
    {
        const ResourceInfo& info = mResourceInfos.at( ResourceType::Info::sType );
        return info.mCount;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void ResourceManager::Get( std::vector<ResourceType*>& _outResources ) const
    {
        const ResourceInfo& info = mResourceInfos.at( ResourceType::Info::sType );
        _outResources.clear();
        _outResources.reserve( (size_t)info.mCount );
        for( auto it : mResources )
        {
            Resource* resource = it.second;
            if( resource->mType == ResourceType::Info::sType )
            {
                _outResources.push_back( static_cast<ResourceType*>(resource) );
            }
        }
        fanAssert( _outResources.size() == info.mCount );
    }
}
