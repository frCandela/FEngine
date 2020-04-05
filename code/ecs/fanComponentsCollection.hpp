#pragma once

#include "fanEcsTypes.hpp"
#include "fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// ComponentsCollection holds an array of chunks that store components.
	// components size are defined in the Init, chunks maximum size is 65536 bytes
	//==============================================================================================================================================================
	class ComponentsCollection
	{
	public:
		//================================
		// Block of memory containing components
		//================================
		struct Chunck
		{
			void* data = nullptr;
			std::vector< ChunckComponentIndex> recycleList;
			ChunckComponentIndex count = 0;
		};

		template< typename _componentType >	void Init( const std::string& _name );
		void*		 At( const ChunckIndex _chunckIndex, const ChunckComponentIndex _componentIndex );
		void		 RemoveComponent( const ChunckIndex _chunckIndex, const ChunckComponentIndex _index );
		Component& NewComponent();

		const std::vector< ComponentsCollection::Chunck >& GetChuncks() const { return m_chunks; }
		const std::string& GetName() const { return m_name; }
		uint32_t GetComponentCount() const { return m_componentCount; }
		uint32_t GetComponentSize() const { return m_componentSize; }
	private:
		std::string m_name = "";
		std::vector< Chunck > m_chunks;
		uint32_t m_componentSize;	// component size in bytes
		uint32_t m_componentCount;	// component count per chunck

		Chunck& AllocChunck();
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	template< typename _componentType >
	void ComponentsCollection::Init( const std::string& _name )
	{
		m_name = _name;
		m_componentSize = sizeof( _componentType );
		m_componentCount = 65536 / m_componentSize; // 65536 bytes is the maximum size of a chunck
	}
}