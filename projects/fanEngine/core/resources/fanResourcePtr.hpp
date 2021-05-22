#pragma once

#include "core/fanHash.hpp"
#include "core/ecs/fanSignal.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct ResourcePtrData
    {
        uint32_t    mType = 0;
        std::string mPath = "";
        Resource                * mResource  = nullptr;
        Signal<ResourcePtrData&>* mOnResolve = nullptr;

        void Resolve()
        {
            if( !mPath.empty() )
            {
                ( *mOnResolve ).Emmit( *this );
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename ResourceType >
    struct ResourcePtr
    {
        static Signal<ResourcePtrData&> sOnResolve;

        ResourcePtr( Resource* _resource = nullptr );
        virtual ~ResourcePtr();
        ResourceType* operator->() const { return (ResourceType*)( mData.mResource ); } //@todo return a reference
        ResourceType* operator*() const { return (ResourceType*)( mData.mResource ); } //@todo return a reference
        bool operator==( const ResourceType* _other ) const { return _other == mData.mResource; }
        bool operator!=( const ResourceType* _other ) const { return _other != mData.mResource; }
        ResourcePtr& operator=( ResourceType* _resource )
        {
            mData.mResource =  _resource;
            return *this;
        }
        bool IsValid() const { return mData.mResource != nullptr; }

        ResourcePtrData mData;
    };

    template< typename _ResourceType >
    Signal<ResourcePtrData&> ResourcePtr<_ResourceType>::sOnResolve;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ResourceType >
    ResourcePtr<_ResourceType>::ResourcePtr( Resource* _resource )
    {
        fanAssert( mData.mResource == nullptr );
        mData.mResource  = _resource;
        mData.mType      = _ResourceType::Info::sType;
        mData.mOnResolve = &sOnResolve;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ResourceType >
    ResourcePtr<_ResourceType>::~ResourcePtr()
    {
    }
}