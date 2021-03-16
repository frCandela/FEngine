#include "core/fanSystem.hpp"
#include "core/fanDebug.hpp"

#ifdef FAN_WIN32
#include <windows.h>
#include "core/fanFile.hpp"

#endif

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    uint64_t System::LastModified( const std::string& _path )
    {
        #ifdef FAN_WIN32
        File file;
        file.Open( _path, File::ReadMode::Read, File::OpenMode::OpenExisting );
        FILETIME creationTime, lastAccessTime, lastWriteTime;
        GetFileTime( (HANDLE)file.Handle(), &creationTime, &lastAccessTime, &lastWriteTime );
        static_assert( sizeof( FILETIME ) == 8 );
        return uint64_t( lastWriteTime.dwHighDateTime ) << 32 | uint64_t( lastWriteTime.dwLowDateTime );
        #endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool System::Exists( const std::string& _path )
    {
        #ifdef FAN_WIN32
        DWORD      fileType    = GetFileAttributes( _path.c_str() );
        const bool isDirectory = fileType & FILE_ATTRIBUTE_DIRECTORY;
        const bool invalid     = INVALID_FILE_ATTRIBUTES == GetFileAttributes( _path.c_str() ) &&
                                 GetLastError() == ERROR_FILE_NOT_FOUND;
        return !invalid && !isDirectory;
        #endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    std::vector<std::string> System::ListDirectory( const std::string& _directoryPath )
    {
        #ifdef  FAN_WIN32
        if( _directoryPath.size() > ( MAX_PATH - 3 ) )
        {
            Debug::Error() << "ListDirectory: directory is too long: " << _directoryPath << Debug::Endl();
            return {};
        }

        std::string     path = _directoryPath + "\\*";
        WIN32_FIND_DATA ffd;

        HANDLE hFind = INVALID_HANDLE_VALUE;
        hFind = FindFirstFile( path.c_str(), &ffd );
        if( hFind == INVALID_HANDLE_VALUE )
        {
            Debug::Error() << "ListDirectory: invalid path: " << _directoryPath << Debug::Endl();
            return {};
        }

        std::vector<std::string> result = {};
        do
        {
            std::string filePath = ffd.cFileName;
            if( filePath != "." && filePath != ".." )
            {
                if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    result.push_back( filePath + "/" );
                }
                else
                {
                    result.push_back( filePath );
                }
            }
        } while( FindNextFile( hFind, &ffd ) != 0 );

        return result;
        #endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool System::HasDebugger()
    {
        #ifdef  FAN_WIN32
        return IsDebuggerPresent();
        #else
        return false;
        #endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool System::StartProcessAndWait( const std::string& _path, const std::string& _args, const std::string& _logsPath )
    {
        #ifdef  FAN_WIN32
        File logsFile;
        logsFile.Open( _logsPath, File::ReadMode::Append, File::OpenMode::CreateNew );

        PROCESS_INFORMATION pi;
        STARTUPINFO         si;
        ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
        ZeroMemory( &si, sizeof( STARTUPINFO ) );

        si.cb         = sizeof( STARTUPINFO );
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput  = NULL;
        si.hStdError  = (HANDLE)logsFile.Handle();
        si.hStdOutput = (HANDLE)logsFile.Handle();

        std::string argsConcat = _path + " " + _args;
        DWORD       flags      = CREATE_NO_WINDOW;
        if( CreateProcess( _path.c_str(), (char*)argsConcat.c_str(), NULL, NULL, TRUE, flags, NULL, NULL, &si, &pi ) )
        {
            WaitForSingleObject( pi.hProcess, INFINITE );
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );
            return true;
        }
        else
        {
            Debug::Error() << "Process creation failed: " << argsConcat << Debug::Endl();
            return false;
        }
        #endif
    }
}