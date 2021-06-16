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
}