#pragma once

#include "fanEcsTypes.hpp"
#include "fanEcComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// ComponentsCollection holds an array of chunks that store components.
	//
	// components size are defined in the Init, chunks maximum size is 65536 bytes
	//==============================================================================================================================================================
	class ComponentsCollection
	{
	public:
		//================================
		template< typename _componentType >
		void Init( const std::string& _name )
		{
			m_name = _name;
			m_componentSize = sizeof( _componentType );
			m_componentCount = 65536 / m_componentSize; // 65536 bytes is the maximum size of a chunck
		}

		//================================
		// Returns the component _componentIndex of chunck _chunckIndex
		void* At( const ChunckIndex _chunckIndex, const ChunckComponentIndex _componentIndex )
		{
			uint8_t* data = static_cast<uint8_t*>( m_chunks[_chunckIndex].data );
			uint8_t* component = data + m_componentSize * (uint32_t)_componentIndex;
			return component;
		}

		//================================
		// Deletes a component
		void RemoveComponent( const ChunckIndex _chunckIndex, const ChunckComponentIndex _index )
		{
			m_chunks[_chunckIndex].recycleList.push_back( _index );
			m_chunks[_chunckIndex].count--;
			if( m_chunks[_chunckIndex].count == 0 )
			{
				m_chunks[_chunckIndex].recycleList.clear();
			}
		}

		//================================
		// Allocates a new component
		ecComponent& NewComponent()
		{
			for( ChunckIndex chunckIndex = 0; chunckIndex < m_chunks.size(); chunckIndex++ )
			{
				Chunck& chunck = m_chunks[chunckIndex];
				if( !chunck.recycleList.empty() )
				{
					// recycle index
					const ChunckComponentIndex index = chunck.recycleList[chunck.recycleList.size() - 1];
					chunck.recycleList.pop_back();
					chunck.count++;
					ecComponent& component = *static_cast<ecComponent*>( At( chunckIndex, index ) );
					component.chunckIndex = chunckIndex;
					component.chunckComponentIndex = index;
					return component;
				}
				else if( chunck.count < m_componentCount )
				{
					// create index
					const ChunckComponentIndex index = chunck.count;
					chunck.count++;
					ecComponent& component = *static_cast<ecComponent*>( At( chunckIndex, index ) );
					component.chunckIndex = chunckIndex;
					component.chunckComponentIndex = index;
					return component;
				}
			}

			// create chunck
			Chunck& newChunck = AllocChunck();
			ecComponent& component = *static_cast<ecComponent*>( At( (ChunckIndex)m_chunks.size() - 1, 0 ) );
			component.chunckIndex = static_cast<ChunckIndex>( m_chunks.size() - 1 );
			component.chunckComponentIndex = 0;
			newChunck.count++;
			return component;
		}

		//private:
		//===============================================================================
		//===============================================================================
		// Block of memory containing components
		struct Chunck
		{
			void* data = nullptr;
			std::vector< ChunckComponentIndex> recycleList;
			ChunckComponentIndex count = 0;
		};

		std::string m_name = "";
		std::vector< Chunck > m_chunks;
		uint32_t m_componentSize;	// component size in bytes
		uint32_t m_componentCount;	// component count per chunck

		Chunck& AllocChunck()
		{
			Chunck chunck;
			chunck.data = malloc( (size_t)m_componentCount * m_componentSize );
			m_chunks.push_back( chunck );
			return m_chunks[m_chunks.size() - 1];
		}
	};
}