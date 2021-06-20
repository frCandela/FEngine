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
    Vector3 Animation::SamplePosition( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyPosition>& positions = mBoneKeys[_boneIndex].mPositions;
        int index = (int)positions.size() - 2;
        for( int i = 0; i < (int)positions.size() - 1; i++ )
        {
            if( _time < positions[i + 1].mTime )
            {
                index = i;
                break;
            }
        }
        const Animation::KeyPosition prevKey = positions[index];
        const Animation::KeyPosition nextKey = positions[index + 1];
         Fixed                  ratio   = ( _time - prevKey.mTime ) / ( nextKey.mTime - prevKey.mTime );
        return ( 1 - ratio ) * prevKey.mPosition + ratio * nextKey.mPosition;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Quaternion Animation::SampleRotation( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyRotation>& rotations = mBoneKeys[_boneIndex].mRotations;
        int index = (int)rotations.size() - 2;
        for( int i = 0; i < (int)rotations.size() - 1; i++ )
        {
            if( _time < rotations[i + 1].mTime )
            {
                index = i;
                break;
            }
        }
        const Animation::KeyRotation prevKey = rotations[index];
        const Animation::KeyRotation nextKey = rotations[index + 1];
        const Fixed                  ratio   = ( _time - prevKey.mTime ) / ( nextKey.mTime - prevKey.mTime );
        return Quaternion::Slerp( prevKey.mRotation, nextKey.mRotation, ratio );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 Animation::SampleScale( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyScale>& scales = mBoneKeys[_boneIndex].mScales;
        int index = (int)scales.size() - 2;
        for( int i = 0; i < (int)scales.size() - 1; i++ )
        {
            if( _time < scales[i + 1].mTime )
            {
                index = i;
                break;
            }
        }
        const Animation::KeyScale prevKey = scales[index];
        const Animation::KeyScale nextKey = scales[index + 1];
        const Fixed               ratio   = ( _time - prevKey.mTime ) / ( nextKey.mTime - prevKey.mTime );
        return ( 1 - ratio ) * prevKey.mScale + ratio * nextKey.mScale;
    }
}