#pragma once

#include "core/resources/fanResources.hpp"
#include "core/math/fanQuaternion.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Animation : Resource
    {
        FAN_RESOURCE(Animation);

        //=================================================================================================
        //=================================================================================================
        struct KeyPosition
        {
            Vector3 mPosition;
            Fixed   mTime;
        };

        //=================================================================================================
        //=================================================================================================
        struct KeyRotation
        {
            Quaternion mRotation;
            Fixed      mTime;
        };

        //=================================================================================================
        //=================================================================================================
        struct KeyScale
        {
            Vector3 mScale;
            Fixed   mTime;
        };

        //=================================================================================================
        //=================================================================================================
        struct BoneAnimation
        {
            std::vector<KeyPosition> mPositions;
            std::vector<KeyRotation> mRotations;
            std::vector<KeyScale>    mScales;
        };

        BoneAnimation mBoneKeys[RenderGlobal::sMaxBones];
        int           mNumBones = 0;
        Fixed         mDuration = 0;

        bool LoadFromFile( const std::string& _path );
        Vector3 SamplePosition( const int _boneIndex, const Fixed _time );
        Quaternion SampleRotation( const int _boneIndex, const Fixed _time );
        Vector3 SampleScale( const int _boneIndex, const Fixed _time );
    };
}