#pragma once

#include <vector>
#include "ecs/fanEcsChunk.hpp"

namespace fan
{
	//================================
	// Dynamic array of fixed size chunks
	// Can be indexed like a continuous array	
	//================================
	class EcsChunkVector
	{
	public:
		void Create( const int _componentSize, const int _alignment )
		{
			m_componentSize = _componentSize;
			m_alignment = _alignment;
			m_chunks.emplace_back();
			m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			m_chunkCapacity = m_chunks.rbegin()->Capacity();
		}

		void Remove( const int& _index )
		{
			const int chunkIndex = _index / m_chunkCapacity;
			const int elementIndex = _index % m_chunkCapacity;
			assert( chunkIndex < int(m_chunks.size()) );
			assert( elementIndex < m_chunks[chunkIndex].Size() );

			EcsChunk& chunk = m_chunks[chunkIndex];

			// Last chunk ? just remove the element & back swap locally in the chunk
			if( chunkIndex == m_chunks.size() - 1 )
			{
				chunk.Remove( elementIndex );
			}
			else
			{
				// back swap the removed element with the last element of the last chunk
				EcsChunk& lastChunk = *m_chunks.rbegin();
				chunk.Set( elementIndex, lastChunk.At( lastChunk.Size() - 1 ) );
				lastChunk.PopBack();
			}

			EcsChunk& lastChunk = *m_chunks.rbegin();
			if( lastChunk.Empty() && m_chunks.size() > 1 )
			{
				lastChunk.Destroy();
				m_chunks.pop_back();
			}
		}

		void* At( const int& _index )
		{
			const int chunkIndex = _index / m_chunkCapacity;
			const int elementIndex = _index % m_chunkCapacity;
			assert( chunkIndex < int(m_chunks.size()) );
			assert( elementIndex < m_chunks[chunkIndex].Size() );

			return m_chunks[chunkIndex].At( elementIndex );
		}

		int PushBack( void* _data )
		{
			// Create a new chunk if necessary
			if( m_chunks.rbegin()->Full() )
			{
				m_chunks.push_back( EcsChunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Push to the last chunk
			EcsChunk& chunk = *m_chunks.rbegin();
			const int chunkIndex = int( m_chunks.size() - 1 );
			const int elementIndex = chunk.Size();
			chunk.PushBack( _data );
			return chunkIndex * m_chunkCapacity + elementIndex;
		}

		int EmplaceBack()
		{
			// Create a new chunk if necessary
			if( m_chunks.rbegin()->Full() )
			{
				m_chunks.push_back( EcsChunk() );
				m_chunks.rbegin()->Create( m_componentSize, m_alignment );
			}

			// Emplace to the last chunk
			EcsChunk& chunk = *m_chunks.rbegin();
			const int chunkIndex = int( m_chunks.size() - 1 );
			const int elementIndex = chunk.Size();
			chunk.EmplaceBack();
			return chunkIndex * m_chunkCapacity + elementIndex;
		}

		void Clear()
		{
			if( m_chunks.empty() )
			{
				return;
			}

			for( int i = NumChunk() - 1; i > 0; i-- )
			{
				m_chunks[i].Destroy();
				m_chunks.pop_back();
			}
			m_chunks[0].Clear();
			assert( m_chunks.size() == 1 );
			assert( m_chunks[0].Empty() );
		}

		const EcsChunk& GetChunk( const int _index ) const { return m_chunks[_index]; }
		EcsChunk&		GetChunk( const int _index ) { return m_chunks[_index]; }
		int				NumChunk() const { return int( m_chunks.size() ); }
	private:
		std::vector<EcsChunk> m_chunks;
		int m_componentSize;
		int m_alignment;
		int m_chunkCapacity;
	};
}