#include <immintrin.h>

inline void* MemcpyCustom( void* _dst, const void* _src, size_t n )
{
	uint8_t* dst = (uint8_t*)_dst;
	const uint8_t* src = (uint8_t*)_src;

    if(n <= 8)
    {
        switch(n)
        {
            case 0:{
                return nullptr;
            }
            case 4:{
                *(uint32_t*)dst = *(uint32_t*)src;
                return nullptr;
            }
            case 8:{
                *(uint64_t*)dst = *(uint64_t*)src;
                return nullptr;
            }
            case 1:{
                *dst = *src;
                return nullptr;
            }
            case 2:{
                *(uint16_t*)dst = *(uint16_t*)src;
                return nullptr;
            }
            case 3:{
                *(uint16_t*)dst = *(uint16_t*)src;
                dst[2] = src[2];
                return nullptr;
            }
            case 5:{
                *(uint32_t*)dst = *(uint32_t*)src;
                dst[4] = src[4];
                return nullptr;
            }
            case 6:{
                *(uint32_t*)dst = *(uint32_t*)src;
                *((uint16_t*)&dst[2])  = *((uint16_t*)&src[2]);
                return nullptr;
            }
            case 7:{
                *(uint32_t*)dst  = *(uint32_t*)src;
                *((uint16_t*)&dst[2])  = *((uint16_t*)&src[2]);
                dst[6] = src[6];
                return nullptr;
            }

            default: return nullptr;
        }
    }

    src += n;
    dst += n;
    int64_t in = -int64_t(n);
//     while (in <= -32)
//     {
//         _mm256_storeu_ps((float*)(dst + in), _mm256_loadu_ps((float*)(src + in)));
//         in += 32;
//     }

    while (in <= -16) 
    {
        _mm_storeu_ps((float*)(dst + in), _mm_loadu_ps((float*)(src + in)));
        in += 16;
    }

    while (in <= -8)
    {
        *((uint64_t*)(dst + in)) = *((uint64_t*)(src + in));
        in += 8;
    }

    while (in <= -4)
    {
        *((uint32_t*)(dst + in)) = *((uint32_t*)(src + in));
        in += 4;
    }

    while (in <= -2)
    {
        *((uint16_t*)(dst + in)) = *((uint16_t*)(src + in));
        in += 2;
    }
    
    while (in)
    {
        *(dst + in) = *(src + in);
        ++in;
    }
    return nullptr;
}