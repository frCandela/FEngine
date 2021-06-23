#pragma once

#include "ecs/fanEcsTypes.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelChunk
    {
        static const int sSize           = 16;
        Fixed            mVoxels[sSize * sSize * sSize];
        glm::ivec3       mPosition       = { -1, -1, -1 };
        EcsHandle        mHandle         = 0;
        bool             mIsMeshOutdated = true;
        bool             mIsGenerated    = false;

        Fixed& operator()( const int _x, const int _y, const int _z )
        {
            return mVoxels[sSize * sSize * _x + sSize * _y + _z];
        }
        const Fixed& operator()( const int _x, const int _y, const int _z ) const
        {
            return mVoxels[sSize * sSize * _x + sSize * _y + _z];
        }
    };
}