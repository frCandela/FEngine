#ifdef  FAN_WIN32

#include <windows.h>
#include "platform/fanFile.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    File::~File()
    {
        Close();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void File::Open( const std::string& _path, const ReadMode _readMode, const OpenMode _openMode )
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength              = sizeof( sa );
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle       = TRUE;

        DWORD readMode = FILE_READ_DATA;
        switch( _readMode )
        {
            case ReadMode::Read :
                readMode = FILE_READ_DATA;
                break;
            case ReadMode::Write :
                readMode = FILE_WRITE_DATA;
                break;
            case ReadMode::Append :
                readMode = FILE_APPEND_DATA;
                break;
        }

        DWORD openMode = OPEN_EXISTING;
        switch( _openMode )
        {
            case OpenMode::OpenExisting :
                openMode = OPEN_EXISTING;
                break;
            case OpenMode::CreateNew :
                openMode = CREATE_NEW;
                break;
        }

        mHandle = (size_t)CreateFileA( _path.c_str(), readMode, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, openMode, FILE_ATTRIBUTE_NORMAL, NULL );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void File::Close()
    {
        if( mHandle != 0 )
        {
            CloseHandle( (HANDLE)mHandle );
            mHandle = 0;
        }
    }
}
#endif