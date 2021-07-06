#ifdef FAN_WIN32

#include <windows.h>
#include <tchar.h>
#include <mutex>
#include "glfw/glfw3.h"
#include "platform/fanSystem.hpp"
#include "platform/fanFile.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    uint64_t System::LastModified( const std::string& _path )
    {
        File file;
        file.Open( _path, File::ReadMode::Read, File::OpenMode::OpenExisting );
        FILETIME creationTime, lastAccessTime, lastWriteTime;
        GetFileTime( (HANDLE)file.Handle(), &creationTime, &lastAccessTime, &lastWriteTime );
        static_assert( sizeof( FILETIME ) == 8 );
        return uint64_t( lastWriteTime.dwHighDateTime ) << 32 | uint64_t( lastWriteTime.dwLowDateTime );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool System::Exists( const std::string& _path )
    {
        DWORD      fileType    = GetFileAttributes( _path.c_str() );
        const bool isDirectory = fileType & FILE_ATTRIBUTE_DIRECTORY;
        const bool invalid     = INVALID_FILE_ATTRIBUTES == GetFileAttributes( _path.c_str() ) &&
                                 GetLastError() == ERROR_FILE_NOT_FOUND;
        return !invalid && !isDirectory;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<std::string> System::ListDirectory( const std::string& _directoryPath )
    {
        if( _directoryPath.size() > ( MAX_PATH - 3 ) )
        {
            Debug::Error() << "ListDirectory: directory is too long: " << _directoryPath << Debug::Endl();
            return {};
        }

        std::string     path = _directoryPath + "*";
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
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool System::HasDebugger()
    {
        return IsDebuggerPresent();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    System::BuildType System::GetBuildType()
    {
        #ifdef  FAN_DEBUG
        return BuildType::Debug;
        #elif  FAN_RELEASE
        return BuildType::Release;
        #else
        fanAssert(false);
        return BuildType::Error;
        #endif
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool System::StartProcessAndWait( const std::string& _path, const std::string& _args, const std::string& _logsPath )
    {
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
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    double System::GetTime()
    {
        return glfwGetTime();
    }

    //==================================================================================================================================================================================================
    // https://developersarea.wordpress.com/2014/09/26/win32-file-watcher-api-to-monitor-directory-changes/
    //==================================================================================================================================================================================================
    bool System::WatchDirectory( const std::string _path, const bool* _enabled, std::vector<std::string>* _outFilesChanged, std::mutex* _mutex )
    {
        fanAssert( _enabled != nullptr && *_enabled == true );
        fanAssert( _outFilesChanged != nullptr && _outFilesChanged->empty() );
        fanAssert( _mutex != nullptr );

        HANDLE hDir = ::CreateFile( _path.c_str(),
                                    FILE_LIST_DIRECTORY,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                    NULL );
        if( hDir == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        OVERLAPPED ovl = { 0 };
        ovl.hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
        if( ovl.hEvent == NULL )
        {
            return false;
        }

        char buffer[1024];

        const DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION;
        const BOOL  watchSubtree = TRUE;
        ::ReadDirectoryChangesW( hDir, buffer, sizeof( buffer ), watchSubtree, notifyFilter, NULL, &ovl, NULL );

        while( *_enabled )
        {
            DWORD dw;
            DWORD result = ::WaitForSingleObject( ovl.hEvent, 100 );

            switch( result )
            {
                case WAIT_OBJECT_0:

                    ::GetOverlappedResult( hDir, &ovl, &dw, FALSE );
                    {
                        DWORD offset             = 0;
                        char  fileName[MAX_PATH] = "";
                        FILE_NOTIFY_INFORMATION* fni = NULL;
                        std::vector<std::string> filesChanged;
                        do
                        {
                            fni = (FILE_NOTIFY_INFORMATION*)( &buffer[offset] );
                            //since we do not use UNICODE, we must convert fni->FileName from UNICODE to multibyte
                            ::WideCharToMultiByte( CP_ACP, 0, fni->FileName, fni->FileNameLength / sizeof( WCHAR ), fileName, sizeof( fileName ), NULL, NULL );

                            switch( fni->Action )
                            {
                                case FILE_ACTION_MODIFIED:
                                    filesChanged.push_back( fileName );
                                    break;
                                default:
                                    filesChanged.push_back( fileName );
                                    break;
                            }

                            ::memset( fileName, '\0', sizeof( fileName ) );
                            offset += fni->NextEntryOffset;
                        } while( fni->NextEntryOffset != 0 );

                        _mutex->lock();
                        _outFilesChanged->insert( _outFilesChanged->end(), filesChanged.begin(), filesChanged.end() );
                        _mutex->unlock();
                    }
                    ::ResetEvent( ovl.hEvent );
                    ::ReadDirectoryChangesW( hDir, buffer, sizeof( buffer ), watchSubtree, notifyFilter, NULL, &ovl, NULL );
                    break;
            }
        }
        return true;
    }
}
#endif