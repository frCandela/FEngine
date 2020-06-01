#include <vector>

namespace fan
{
	//================================================================================================================================
	// Allocates chunks of a fixed size for use in the ecs archetypes.
	// Manages a pool of chunks to allow easy reuse without unnecessary memory allocation
	//================================================================================================================================
	class EcsChunkAllocator
	{
	public:
		static constexpr size_t chunkSize = 65536;

		//================================================================
		//================================================================	
		~EcsChunkAllocator()
		{
			assert( m_size == 0 );
			for ( void* chunk : m_freeChunks )
			{
				delete chunk;
			}
			m_freeChunks.clear();
		}

		//================================================================
		//================================================================
		void* Alloc()
		{
			void* chunk = nullptr;
			if( m_freeChunks.empty() )
			{
				chunk = new uint8_t[chunkSize];
				m_size ++;
			}
			else
			{
				chunk = *m_freeChunks.rbegin();
				m_freeChunks.pop_back();
			}
			return chunk;
		}

		//================================================================
		//================================================================
		void Free( void * _chunk )
		{
			assert( _chunk != nullptr );
			m_freeChunks.push_back( _chunk );
		}

		//================================================================
		//================================================================
		size_t Size() const { return m_size; }

	private:
		std::vector<void*> m_freeChunks;
		size_t m_size;
	};
}