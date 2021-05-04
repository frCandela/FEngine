#pragma once

#include "render/resources/fanMesh.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelChunk
    {
        static const int sSize           = 16;
        bool             mVoxels[sSize][sSize][sSize];
        glm::ivec3       mPosition       = { -1, -1, -1 };
        EcsHandle        mHandle         = 0;
        bool             mIsMeshOutdated = true;
        bool             mIsGenerated    = false;
    };
}