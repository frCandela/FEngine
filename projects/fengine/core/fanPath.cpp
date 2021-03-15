#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"

#include <sstream>

namespace fan
{
    std::string Path::sProjectPath = "";

    //==========================================================================================================================
    //==========================================================================================================================
    bool Path::SetProjectPath( const std::string& _projectPath )
    {
        fanAssertMsg( sProjectPath.empty(), "project path assigned multiple times" );
        if( !_projectPath.empty() && IsAbsolute( _projectPath ) )
        {
            sProjectPath = Normalize( _projectPath );
            if( *sProjectPath.rbegin() != '/' )
            {
                sProjectPath += '/';
            }
            Debug::Log() << "Project path: " << sProjectPath << Debug::Endl();
            return true;
        }
        Debug::Warning() << "Path::SetProjectPath: invalid project path : " << _projectPath << Debug::Endl();
        return false;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void Path::Reset()
    {
        sProjectPath = "";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool Path::IsAbsolute( const std::string& _path )
    {
        if( _path.size() < 2 ){ return false; }
        return _path[1] == ':';
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool Path::IsRootDrive(const std::string& _path )
    {
        return _path.size() == 3 && _path[1] == ':';
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool Path::IsDirectory( const std::string& _path )
    {
        return _path.empty() || *( _path.rbegin() ) == '/';
    }

    //==========================================================================================================================
    // removes backwards slashes and doubles slashes
    //==========================================================================================================================
    std::string ConvertInvalidSlashes( const std::string& _path )
    {
        std::stringstream ss;
        char              previousLetter = ' ';
        for( int          i              = 0; i < (int)_path.size(); ++i )
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
        return ss.str();
    }

    //==========================================================================================================================
    // removes backward slashes a
    // removes double slashes
    // makes absolute
    //==========================================================================================================================
    std::string Path::Normalize( const std::string& _path )
    {
        if( _path.empty() ){ return ""; }

        std::string normalizedPath = ConvertInvalidSlashes( _path );

        // removes leading slash
        if( !normalizedPath.empty() && *normalizedPath.begin() == '/' )
        {
            normalizedPath.erase( normalizedPath.begin() );
        }

        if( normalizedPath.empty() || !IsAbsolute( normalizedPath ) )
        {
            normalizedPath = sProjectPath + "content/" + normalizedPath;
        }
        return normalizedPath;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::string Path::Directory( const std::string& _path )
    {
        std::string directory = _path;
        while( !directory.empty() )
        {
            if( *directory.rbegin() == '/' )
            {
                return directory;
            }
            else
            {
                directory.pop_back();
            }
        }
        return "/";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::string Path::FileName( const std::string& _path )
    {
        for( int i = (int)_path.size() - 1; i >= 0; --i )
        {
            if( _path[i] == '/' )
            {
                return std::string( _path.begin() + i + 1, _path.end() );
            }
        }
        return _path;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::string Path::Parent( const std::string& _path )
    {
        if( _path.empty() ){ return "/"; }

        std::string parent = _path;
        if( IsDirectory( parent ) )
        {
            if( IsRootDrive( parent ) )
            {
                return parent;
            }
            parent.pop_back();
        }
        return Directory( parent );
    }
}