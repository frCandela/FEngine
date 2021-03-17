#include "core/fanPath.hpp"

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
            return true;
        }
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
    bool Path::IsRootDrive( const std::string& _path )
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
    std::string Path::NormalizeSlashes( const std::string& _path )
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
        std::string path = ss.str();

        // removes leading slash
        if( !path.empty() && *path.begin() == '/' )
        {
            path.erase( path.begin() );
        }

        return path;
    }

    //==========================================================================================================================
    // removes backward slashes a
    // removes double slashes
    // makes absolute
    //==========================================================================================================================
    std::string Path::Normalize( const std::string& _path )
    {
        if( _path.empty() ){ return ""; }

        std::string normalizedPath = NormalizeSlashes( _path );

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
    std::string Path::Extension( const std::string& _path )
    {
        for( int i = (int)_path.size() - 1; i >= 0; --i )
        {
            if( _path[i] == '/' )
            {
                return "";
            }
            else if( _path[i] == '.' )
            {
                return std::string( _path.begin() + i + 1, _path.end() );
            }
        }
        return "";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::string Path::FileName( const std::string& _path )
    {
        int endPosition = (int)_path.size() - 1;
        if( IsDirectory( _path ) )
        {
            endPosition -= 1; // skip trailing '/'
        }

        // get filename
        std::string filenameWithExtension = _path;
        for( int i = endPosition; i >= 0; --i )
        {
            if( _path[i] == '/'  )
            {
                filenameWithExtension = std::string( _path.begin() + i + 1, _path.end() );
                break;
            }
        }

        int fileNameSize = (int)filenameWithExtension.size();
        std::string extension = Extension( filenameWithExtension );
        if( !extension.empty() )
        {
            fileNameSize -= int(extension.size()) + 1; // don't forget the .
        }
        return std::string( filenameWithExtension.begin(), filenameWithExtension.begin() + fileNameSize );
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

    //==========================================================================================================================
    //==========================================================================================================================
    std::vector<std::string> Path::ListDirectory( const std::string& _directoryPath )
    {
        if( !IsDirectory( _directoryPath ) )
        {
            return {};
        }
        std::vector<std::string> childPaths = System::ListDirectory( _directoryPath );
        for( std::string& childPath : childPaths )
        {
            childPath = _directoryPath + childPath;
        }
        return childPaths;
    }
}