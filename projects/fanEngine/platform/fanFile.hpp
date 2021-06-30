#pragma once

#include <string>

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class File
    {
    public:
        enum class ReadMode
        {
            Read, Write, Append
        };
        enum class OpenMode
        {
            OpenExisting, CreateNew
        };

        ~File();
        void Open( const std::string& _path, const ReadMode _readMode, const OpenMode _openMode );
        void Close();
        size_t Handle() { return mHandle; }

    private:
        #ifdef  FAN_WIN32
        size_t mHandle = 0; //win32
        #endif
    };
}