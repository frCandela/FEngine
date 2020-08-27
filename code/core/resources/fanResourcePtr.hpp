#pragma once

#include "core/fanHash.hpp"
#include "ecs/fanSignal.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	template< typename ResourceType>
	class ResourcePtr
	{
	public:
		static Signal< ResourcePtr<ResourceType>& > sOnResolve;

		ResourcePtr( Resource* _resource = nullptr );
		virtual ~ResourcePtr();

		void			Resolve() { SetResource( nullptr ); sOnResolve.Emmit( *this ); }
		bool			IsValid()       const { return mResource != nullptr; }
		ResourceType*  GetResource()   const { return static_cast< ResourceType* >( mResource ); }

		ResourceType* operator->() const { return ( ResourceType* ) ( mResource ); } //@todo return a reference
		ResourceType* operator*()  const { return ( ResourceType* ) ( mResource ); } //@todo return a reference
		bool operator==( const ResourceType* _other ) const { return _other == mResource; }
		bool operator!=( const ResourceType* _other ) const { return _other != mResource; }
	protected:
		void SetResource(Resource* _resource);

	private:
		Resource* mResource = nullptr;

	};

	template< typename _ResourceType>
	Signal< ResourcePtr<_ResourceType>& > ResourcePtr<_ResourceType>::sOnResolve;

	//========================================================================================================
	//========================================================================================================
	template< typename _ResourceType >
	ResourcePtr<_ResourceType>::ResourcePtr( Resource* _resource ) :
		mResource( _resource )
	{		
		assert( mResource == nullptr );
		SetResource( _resource );
	}

	//========================================================================================================
	//========================================================================================================
	template< typename _ResourceType >
	void ResourcePtr<_ResourceType>::SetResource( Resource * _resource ) 
	{
		if ( _resource == mResource ) { return; }

		if ( mResource != nullptr )
		{ 
			mResource->DecreaseRefCount();
            mResource = nullptr;
		}

		if ( _resource != nullptr )
		{
            mResource = _resource;
			mResource->IncreaseRefCount();
		}
	}

	//========================================================================================================
	//========================================================================================================
	template< typename _ResourceType >
	ResourcePtr<_ResourceType>::~ResourcePtr()
	{
		if ( mResource != nullptr )
		{
			mResource->DecreaseRefCount();
		}
	}
}