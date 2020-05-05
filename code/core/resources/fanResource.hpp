#pragma once

namespace fan
{
	template< typename T > class ResourceList;
	template< typename T > class ResourcePtr;

	//================================================================================================================================
	// Base class for resources ( Mesh, textures etc.) 
	//================================================================================================================================
	class Resource
	{
	public:
		bool IsReferenced() const { return m_refCount > 0; }
		int  GetRefCount() const { return m_refCount;  }

		void IncreaseRefCount()  { m_refCount ++; }
		void DecreaseRefCount()  { m_refCount --; }
	private:
		int m_refCount = 0;
	};
}