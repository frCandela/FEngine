#pragma once

#include "render/resources/fanMesh.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelChunk
    {
        static const int sSize           = 16;
        uint8_t          mVoxels[sSize * sSize * sSize];
        glm::ivec3       mPosition       = { -1, -1, -1 };
        EcsHandle        mHandle         = 0;
        bool             mIsMeshOutdated = true;
        bool             mIsGenerated    = false;

        uint8_t& operator()( const int _x, const int _y, const int _z )
        {
            return mVoxels[sSize * sSize * _x + sSize * _y + _z];
        }
        const uint8_t& operator()( const int _x, const int _y, const int _z ) const
        {
            return mVoxels[sSize * sSize * _x + sSize * _y + _z];
        }
    };
}