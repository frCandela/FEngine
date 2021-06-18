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
        if( _time.ToInt() >= positions.size() )
        {
            return Vector3::sZero;
        }
        return positions.empty() ? Vector3::sZero : positions[_time.ToInt()].mPosition;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Quaternion Animation::SampleRotation( const int _boneIndex, const Fixed _time )
    {
        const std::vector<Animation::KeyRotation>& rotations = mBoneKeys[_boneIndex].mRotations;
        if( _time.ToInt() >= rotations.size() )
        {
            return Quaternion::sIdentity;
        }
        return rotations.empty() ? Quaternion::sIdentity : rotations[_time.ToInt()].mRotation;
    }
}