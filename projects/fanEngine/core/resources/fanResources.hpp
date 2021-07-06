#pragma once

#include <vector>
#include <map>
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

        // set this to change behaviour
        bool mUseDestroyList = false;
        bool mUseDirtyList   = false;
        void* mDataPtr = nullptr;

        // internal data
        int mCount = 0;

        std::vector<Resource*> mDestroyList;
        std::vector<Resource*> mDirtyList;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class Resources
    {
    public:
        template< class ResourceType >
        void AddResourceType( const ResourceInfo& _resourceInfo );

        template< class ResourceType >
        ResourcePtr <ResourceType> Load( const std::string& _path );
        ResourcePtrData Load( uint32_t _type, const std::string& _path );

        template< class ResourceType >
        ResourcePtr <ResourceType> Add( ResourceType* _resource, const std::string& _path );

        template< class ResourceType >
        ResourcePtr <ResourceType> Get( const std::string& _path );
        template< class ResourceType >
        ResourcePtr <ResourceType> Get( const uint32_t _guid );
        ResourcePtrData Get( const std::string& _path );
        ResourcePtrData Get( const uint32_t _guid );

        template< class ResourceType >
        void Get( std::vector<ResourcePtr < ResourceType>>        & _outResources );

        template< class ResourceType >
        ResourcePtr <ResourceType> GetOrLoad( const std::string& _path );
        ResourcePtrData GetOrLoad( const uint32_t _type, const std::string& _path );

        bool Remove( const std::string& _path );
        bool Remove( const uint32_t _guid );

        bool SetDirty( const uint32_t _guid );

        template< class ResourceType >
        void GetDestroyList( std::vector<ResourceType*>& _destroyedList );

        template< class ResourceType >
        void GetDirtyList( std::vector<ResourceType*>& _outdatedList );

        template< class ResourceType >
        int Count() const;

        void Clear();

        uint32_t GetUniqueID() { return mNextUniqueID++;}

    private:
        ResourceHandle* LoadInternal( const uint32_t _type, const std::string& _path );
        ResourceHandle* AddInternal( const uint32_t _type, Resource* _resource, const std::string& _path );
        ResourceHandle* GetInternal( const uint32_t _guid );
        ResourceHandle* GetOrLoadInternal( const uint32_t _type, const std::string& _path );
        bool DeleteResource( std::map<uint32_t, ResourceHandle*>::iterator _iterator );

        std::map<uint32_t, Resource*>       mResources;
        std::map<uint32_t, ResourceHandle*> mResourceHandles;
        std::map<uint32_t, ResourceInfo>    mResourceInfos;
        uint32_t mNextUniqueID = 0;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void Resources::AddResourceType( const ResourceInfo& _resourceInfo )
    {
        fanAssert( mResourceInfos.find( ResourceType::Info::sType ) == mResourceInfos.end() );
        ResourceInfo resourceInfo = _resourceInfo;
        resourceInfo.mCount = 0;
        mResourceInfos[ResourceType::Info::sType] = resourceInfo;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourcePtr <ResourceType> Resources::Load( const std::string& _path )
    {
        ResourcePtr<ResourceType> resourcePtr;
        resourcePtr.mData.mHandle = LoadInternal( ResourceType::Info::sType, _path );
        resourcePtr.mData.mGUID   = resourcePtr.mData.mHandle != nullptr ? resourcePtr.mData.mHandle->mResource->mGUID : 0;
        return resourcePtr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourcePtr <ResourceType> Resources::Add( ResourceType* _resource, const std::string& _path )
    {
        ResourcePtr<ResourceType> resourcePtr;
        resourcePtr.mData.mHandle = AddInternal( ResourceType::Info::sType, _resource, _path );
        resourcePtr.mData.mGUID   = resourcePtr.mData.mHandle != nullptr ? resourcePtr.mData.mHandle->mResource->mGUID : 0;
        return resourcePtr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourcePtr <ResourceType> Resources::Get( const std::string& _path )
    {
        return Get<ResourceType>( DSID( _path.c_str() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourcePtr <ResourceType> Resources::Get( const uint32_t _guid )
    {
        ResourcePtr<ResourceType> resourcePtr;
        resourcePtr.mData = Get( _guid );
        fanAssert( resourcePtr.mData.mHandle == nullptr || resourcePtr.mData.mHandle->mResource == nullptr || ResourceType::Info::sType == resourcePtr.mData.mHandle->mResource->mType );
        return resourcePtr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    ResourcePtr <ResourceType> Resources::GetOrLoad( const std::string& _path )
    {
        ResourcePtr<ResourceType> resourcePtr;
        resourcePtr.mData = GetOrLoad( ResourceType::Info::sType, _path );
        return resourcePtr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void Resources::GetDestroyList( std::vector<ResourceType*>& _destroyedList )
    {
        ResourceInfo& info = mResourceInfos.at( ResourceType::Info::sType );
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
    void Resources::GetDirtyList( std::vector<ResourceType*>& _outdatedList )
    {
        ResourceInfo& info = mResourceInfos.at( ResourceType::Info::sType );
        fanAssert( info.mUseDirtyList );
        _outdatedList.clear();
        for( Resource* resource : info.mDirtyList )
        {
            fanAssert( resource->mIsDirty );
            resource->mIsDirty = false;
            _outdatedList.push_back( static_cast<ResourceType*>(resource) );
        }
        info.mDirtyList.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    int Resources::Count() const
    {
        const ResourceInfo& info = mResourceInfos.at( ResourceType::Info::sType );
        return info.mCount;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class ResourceType >
    void Resources::Get( std::vector<ResourcePtr < ResourceType>> & _outResources )
    {
        const ResourceInfo& info = mResourceInfos.at( ResourceType::Info::sType );
        _outResources.clear();
        _outResources.reserve( info.mCount );
            for(auto it : mResourceHandles )
            {
                ResourceHandle* handle = it.second;
                if( handle->mResource != nullptr && handle->mResource->mType == ResourceType::Info::sType&& handle->mResource != nullptr )
                {
                    ResourcePtr <ResourceType> resourcePtr;
                    resourcePtr.mData.
                    mHandle = handle;
                    resourcePtr.mData.
                    mGUID = resourcePtr.mData.mHandle->mResource->mGUID;
                    _outResources.
                    push_back( resourcePtr );
                }
            }
            fanAssert( _outResources.size() == info.mCount );
        }
    }
