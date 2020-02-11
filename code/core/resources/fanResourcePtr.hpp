#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/resources/fanResource.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	template< typename _ResourceType>
	class ResourcePtr
	{
	public:
		friend class ResourceList<_ResourceType>;

		ResourcePtr( Resource<_ResourceType>* _resourceType = nullptr );

		bool			IsValid()      const { return m_resource != nullptr; }
		_ResourceType* GetResource() const { return static_cast< _ResourceType* >( m_resource ); }

		_ResourceType* operator->() const { return ( _ResourceType* ) ( m_resource ); }
		_ResourceType* operator*()  const { return ( _ResourceType* ) ( m_resource ); }

	private:
		Resource<_ResourceType>* m_resource = nullptr;
	};



	//================================================================================================================================
	// Creates a resourcePtr referencing an already existing resource
	//================================================================================================================================
	template< typename _ResourceType >
	ResourcePtr<_ResourceType>::ResourcePtr( Resource<_ResourceType>* _resourceType ) :
		m_resource( _resourceType )
	{
		if ( _resourceType != nullptr )
		{
			++_resourceType->m_refCount;
		}
	}
}