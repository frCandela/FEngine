#pragma once

#include "core/fanString.hpp"
#include "core/math/fanMatrix4.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Bone
    {
        static const int sMaxChilds = 8;
        int              mChilds[sMaxChilds];
        int              mNumChilds = 0;
        String<32>       mName;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Skeleton
    {
        Bone       mBones[RenderGlobal::sMaxBones];
        Matrix4    mOffsetMatrix[RenderGlobal::sMaxBones];
        Matrix4    mInverseBindMatrix[RenderGlobal::sMaxBones];
        Matrix4    mRootTransform;
        int        mNumBones = 0;
        String<32> mName;
    };
}