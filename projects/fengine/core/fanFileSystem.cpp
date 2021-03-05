#include "core/fanFileSystem.hpp"

#include <sstream>

namespace fan
{
    std::string FileSystem::s_projectPath = "";

    //========================================================================================================
    //========================================================================================================
    bool FileSystem::SetProjectPath( const std::string& _projectPath )
    {
        fanAssertMsg( s_projectPath.empty(), "project path assigned multiple times" );
        if( !_projectPath.empty() && IsAbsolute(s_projectPath) )
        {
            s_projectPath = NormalizePath( _projectPath );
            if( *s_projectPath.rbegin() != '/' )
            {
                s_projectPath += '/';
            }
            s_projectPath += "content";
            return true;
        }

        fanAssertMsg( false, "invalid project path" );
        return false;
    }

    //========================================================================================================
    //========================================================================================================
    void FileSystem::Reset()
    {
        s_projectPath = "";
    }

    //========================================================================================================
    //========================================================================================================
    bool FileSystem::IsAbsolute( const std::string& _path )
    {
        if( _path.empty() ){ return true; }
        if( _path.size() < 2 ){ return false; }
        return _path[1] == ':';
    }

    //========================================================================================================
    // removes backward slashes a
    // removes double slashes
    // makes absolute
    //========================================================================================================
    std::string FileSystem::NormalizePath( const std::string& _path )
    {
        if( _path.empty() ) { return ""; }

        std::stringstream ss;

        // removes backwards slashes and doubles slashes
        char     previousLetter = ' ';
        for( int i              = 0; i < _path.size(); ++i )
        {
            const char currentLetter = _path[i];
            char       normalizedLetter;
            switch( currentLetter )
            {
                case '\\':
                case '/':
                    normalizedLetter = '/';
                    break;
                default:
                    normalizedLetter = currentLetter;
                    break;
            }
            if( previousLetter != '/' || normalizedLetter != '/' )
            {
                ss << normalizedLetter;
                previousLetter = normalizedLetter;
            }
        }

        std::string normalizedPath = ss.str();

        // removes trailing slash
        if( *normalizedPath.rbegin() == '/' )
        {
            normalizedPath.pop_back();
        }

        // removes leading slash
        if( *normalizedPath.begin() == '/' )
        {
            normalizedPath.erase( normalizedPath.begin() );
        }

        if( !s_projectPath.empty() && !IsAbsolute( normalizedPath ) )
        {
            normalizedPath = s_projectPath + '/' + normalizedPath;
        }
        return normalizedPath;
    }
}