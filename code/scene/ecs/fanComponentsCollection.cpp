#include "scene/ecs/fanComponentsCollection.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// returns the component _componentIndex of chunck _chunckIndex
	//==============================================================================================================================================================
	void* ComponentsCollection::At( const ChunckIndex _chunckIndex, const ChunckComponentIndex _componentIndex )
	{
		uint8_t* data = static_cast<uint8_t*>( m_chunks[_chunckIndex].data );
		uint8_t* component = data + m_componentSize * (uint32_t)_componentIndex;
		return component;
	}


	//==============================================================================================================================================================
	// deletes a component
	//==============================================================================================================================================================
	void ComponentsCollection::RemoveComponent( const ChunckIndex _chunckIndex, const ChunckComponentIndex _index )
	{
		m_chunks[_chunckIndex].recycleList.push_back( _index );
		m_chunks[_chunckIndex].count--;
		if( m_chunks[_chunckIndex].count == 0 )
		{
			m_chunks[_chunckIndex].recycleList.clear();
		}
	}


	//==============================================================================================================================================================
	// allocates a new component
	//==============================================================================================================================================================
	Component& ComponentsCollection::NewComponent()
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
				Component& component = *static_cast<Component*>( At( chunckIndex, index ) );
				component.chunckIndex = chunckIndex;
				component.chunckComponentIndex = index;
				return component;
			}
			else if( chunck.count < m_componentCount )
			{
				// create index
				const ChunckComponentIndex index = chunck.count;
				chunck.count++;
				Component& component = *static_cast<Component*>( At( chunckIndex, index ) );
				component.chunckIndex = chunckIndex;
				component.chunckComponentIndex = index;
				return component;
			}
		}

		// create chunck
		Chunck& newChunck = AllocChunck();
		Component& component = *static_cast<Component*>( At( (ChunckIndex)m_chunks.size() - 1, 0 ) );
		component.chunckIndex = static_cast<ChunckIndex>( m_chunks.size() - 1 );
		component.chunckComponentIndex = 0;
		newChunck.count++;
		return component;
	}

	//==============================================================================================================================================================
	//
	//==============================================================================================================================================================
	ComponentsCollection::Chunck& ComponentsCollection::AllocChunck()
	{
		Chunck chunck;
		chunck.data = malloc( (size_t)m_componentCount * m_componentSize );
		m_chunks.push_back( chunck );
		return m_chunks[m_chunks.size() - 1];
	}
}