#include "render/resources/fanAnimation.hpp"
#include "render/fanGLTFImporter.hpp"
#include "core/fanDebug.hpp"
#include "core/fanPath.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Animation::LoadFromFile( const std::string& _path )
    {
        if( _path.empty() )
        {
            Debug::Warning() << "Trying to load animation with an empty path" << Debug::Endl();
            return false;
        }

        GLTFImporter importer;
        if( importer.Load( Path::Normalize( _path ) ) )
        {
            importer.GetAnimation( *this );
            mPath = _path;
            return true;
        }

        Debug::Warning() << "Failed to load animation : " << mPath << Debug::Endl();
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed GetSampleRatio( const Fixed _prevTime, const Fixed _nextTime, const Fixed _time )
    {
        const Fixed framesDiff = _nextTime - _prevTime;
        const Fixed length     = Fixed::Clamp( _time - _prevTime, 0, framesDiff );
        const Fixed ratio      = length / framesDiff;
        fanAssert( ratio >= 0 && ratio <= 1 );
        return ratio;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 Animation::SamplePosition( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyPosition>& positions = mBoneKeys[_boneIndex].mPositions;
        if( positions.empty() )
        {
            return Vector3::sZero;
        }
        else if( positions.size() == 1 )
        {
            return positions[0].mPosition;
        }
        else
        {
            int                          index       = (int)positions.size() - 2;
            for( int                     i           = 0; i < (int)positions.size() - 1; i++ )
            {
                if( _time < positions[i + 1].mTime )
                {
                    index = i;
                    break;
                }
            }
            const Animation::KeyPosition prevKey     = positions[index];
            const Animation::KeyPosition nextKey     = positions[index + 1];
            const Fixed                  sampleRatio = GetSampleRatio( prevKey.mTime, nextKey.mTime, _time );
            return ( 1 - sampleRatio ) * prevKey.mPosition + sampleRatio * nextKey.mPosition;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Quaternion Animation::SampleRotation( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyRotation>& rotations = mBoneKeys[_boneIndex].mRotations;
        if( rotations.empty() )
        {
            return Quaternion::sIdentity;
        }
        else if( rotations.size() == 1 )
        {
            return rotations[0].mRotation;
        }
        else
        {
            int                          index       = (int)rotations.size() - 2;
            for( int                     i           = 0; i < (int)rotations.size() - 1; i++ )
            {
                if( _time < rotations[i + 1].mTime )
                {
                    index = i;
                    break;
                }
            }
            const Animation::KeyRotation prevKey     = rotations[index];
            const Animation::KeyRotation nextKey     = rotations[index + 1];
            const Fixed                  sampleRatio = GetSampleRatio( prevKey.mTime, nextKey.mTime, _time );
            return Quaternion::Slerp( prevKey.mRotation, nextKey.mRotation, sampleRatio );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 Animation::SampleScale( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyScale>& scales = mBoneKeys[_boneIndex].mScales;
        if( scales.empty() )
        {
            return Vector3::sOne;
        }
        else if( scales.size() == 1 )
        {
            return scales[0].mScale;
        }
        else
        {
            int                       index       = (int)scales.size() - 2;
            for( int                  i           = 0; i < (int)scales.size() - 1; i++ )
            {
                if( _time < scales[i + 1].mTime )
                {
                    index = i;
                    break;
                }
            }
            const Animation::KeyScale prevKey     = scales[index];
            const Animation::KeyScale nextKey     = scales[index + 1];
            const Fixed               sampleRatio = GetSampleRatio( prevKey.mTime, nextKey.mTime, _time );
            return ( 1 - sampleRatio ) * prevKey.mScale + sampleRatio * nextKey.mScale;
        }
    }
}