#include <vector>
#include <cstdint>

namespace fan
{
    //==================================================================================================================================================================================================
    // Allocates chunks of a fixed size for use in the ecs archetypes.
    // Manages a pool of chunks to allow easy reuse without unnecessary memory allocation
    //==================================================================================================================================================================================================
    class EcsChunkAllocator
    {
    public:
        static constexpr size_t sChunkSize = 65536;

        ~EcsChunkAllocator();
        void* Alloc();
        void Free( void* _chunk );
        size_t Size() const { return mSize; }

    private:
        std::vector<void*> mFreeChunks;
        size_t             mSize;
    };
}