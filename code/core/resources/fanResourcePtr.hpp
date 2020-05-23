#pragma once

#include "core/fanHash.hpp"
#include "core/fanSignal.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	template< typename _ResourceType>
	class ResourcePtr
	{
	public:
		static Signal< ResourcePtr<_ResourceType>& > s_onResolve;

		ResourcePtr( Resource* _resource = nullptr );
		virtual ~ResourcePtr();

		void			Resolve() { SetResource( nullptr ); s_onResolve.Emmit( *this ); }
		bool			IsValid()       const { return m_resource != nullptr; }
		_ResourceType*  GetResource()   const { return static_cast< _ResourceType* >( m_resource ); }

		_ResourceType* operator->() const { return ( _ResourceType* ) ( m_resource ); } //@todo return a reference
		_ResourceType* operator*()  const { return ( _ResourceType* ) ( m_resource ); } //@todo return a reference
		bool operator==( const _ResourceType* _other ) const { return _other == m_resource; }
		bool operator!=( const _ResourceType* _other ) const { return _other != m_resource; }		
	protected:
		void SetResource(Resource* _resource);

	private:
		Resource* m_resource = nullptr;

	};

	template< typename _ResourceType>
	Signal< ResourcePtr<_ResourceType>& > ResourcePtr<_ResourceType>::s_onResolve;

	//================================================================================================================================
	//================================================================================================================================
	template< typename _ResourceType >
	ResourcePtr<_ResourceType>::ResourcePtr( Resource* _resource ) :
		m_resource( _resource )
	{		
		assert( m_resource == nullptr );
		SetResource( _resource );
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _ResourceType >
	void ResourcePtr<_ResourceType>::SetResource( Resource * _resource ) 
	{
		if ( _resource == m_resource ) { return; }

		if ( m_resource != nullptr ) 
		{ 
			m_resource->DecreaseRefCount(); 
			m_resource = nullptr;
		}

		if ( _resource != nullptr )
		{
			m_resource = _resource;
			m_resource->IncreaseRefCount();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _ResourceType >
	ResourcePtr<_ResourceType>::~ResourcePtr()
	{
		if ( m_resource != nullptr )
		{
			m_resource->DecreaseRefCount();
		}
	}
}