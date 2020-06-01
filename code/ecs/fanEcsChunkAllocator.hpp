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

		~EcsChunkAllocator();
		void*	Alloc();
		void	Free( void* _chunk );
		size_t	Size() const { return m_size; }

	private:
		std::vector<void*> m_freeChunks;
		size_t m_size;
	};
}