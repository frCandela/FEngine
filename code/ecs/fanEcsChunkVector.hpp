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
		void	Create( void* ( *_cpyFunction )( void*, const void*, size_t ), const int _componentSize, const int _alignment );
		void	Remove( const int& _index );
		void*	At( const int& _index );
		int		PushBack( void* _data );
		int		EmplaceBack();
		void	Clear();

		const EcsChunk& GetChunk( const int _index ) const	{ return m_chunks[_index];		}
		EcsChunk&		GetChunk( const int _index )		{ return m_chunks[_index];		}
		int				NumChunk() const					{ return int( m_chunks.size() );}

	private:
		std::vector<EcsChunk> m_chunks;
		int m_componentSize;
		int m_alignment;
		int m_chunkCapacity;
		void* ( *m_cpyFunction )( void*, const void*, size_t ) = nullptr;
	};
}