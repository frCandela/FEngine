#include "core/fanFileSystem.hpp"
#include "core/fanDebug.hpp"

#include <sstream>

namespace fan
{
    std::string FileSystem::sProjectPath = "";

    //==========================================================================================================================
    //==========================================================================================================================
    bool FileSystem::SetProjectPath( const std::string& _projectPath )
    {
        fanAssertMsg( sProjectPath.empty(), "project path assigned multiple times" );
        if( !_projectPath.empty() && IsAbsolute( sProjectPath ) )
        {
            sProjectPath = NormalizePath( _projectPath );
            if( *sProjectPath.rbegin() != '/' )
            {
                sProjectPath += '/';
            }
            Debug::Log() << "Project path:" <<  sProjectPath << Debug::Endl();
            return true;
        }

        fanAssertMsg( false, "invalid project path" );
        return false;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void FileSystem::Reset()
    {
        sProjectPath = "";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool FileSystem::IsAbsolute( const std::string& _path )
    {
        if( _path.empty() ){ return true; }
        if( _path.size() < 2 ){ return false; }
        return _path[1] == ':';
    }

    //==========================================================================================================================
    // removes backward slashes a
    // removes double slashes
    // makes absolute
    //==========================================================================================================================
    std::string FileSystem::NormalizePath( const std::string& _path )
    {
        if( _path.empty() ){ return ""; }

        std::stringstream ss;

        // removes backwards slashes and doubles slashes
        char     previousLetter = ' ';
        for( int i              = 0; i < (int)_path.size(); ++i )
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
        if( ! normalizedPath.empty() && *normalizedPath.begin() == '/' )
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
    std::string FileSystem::Directory( const std::string& _path )
    {
        std::string directory = _path;
        while ( !directory.empty() )
        {
            if( *directory.rbegin()  == '/' )
            {
                return directory;
            }
            else
            {
                directory.pop_back();
            }
        }
        return "";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::string FileSystem::FileName( const std::string& _path )
    {
        for( int i = (int)_path.size() - 1; i >= 0; --i )
        {
            if ( _path[i] == '/' )
            {
                return std::string(_path.begin() + i + 1, _path.end());
            }
        }
        return "";
    }
}