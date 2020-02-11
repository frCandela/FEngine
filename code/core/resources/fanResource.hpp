#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{

	template< typename T > class ResourceList;
	template< typename T > class ResourcePtr;

	//================================================================================================================================
	// Base class for resources ( Mesh, textures, components, gameobjects etc.) 
	//================================================================================================================================
	template< typename _ResourceType >
	class Resource
	{
	public:
		friend class ResourceList<_ResourceType>;
		friend class ResourcePtr<_ResourceType>;

		static ResourceList<_ResourceType> s_list;

		bool IsReferenced() const { return m_refCount > 0; }
	private:
		int m_refCount;
	};
}