#pragma once

#include <string>

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class IFile
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
        virtual void Open( const std::string& _path, const ReadMode _readMode, const OpenMode _openMode ) = 0;
        virtual void Close() = 0;
        virtual size_t Handle() = 0;
        virtual ~IFile() {};
    };
}