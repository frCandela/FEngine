#pragma once

#ifdef FAN_WIN32

#include "core/platforms/fanIFile.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    class WinFile : public IFile
    {
    public:
        void Open( const std::string& _path, const ReadMode _readMode, const OpenMode _openMode ) override;
        void Close() override;
        size_t Handle() override { return mHandle; }
        ~WinFile() override;

    private:
        size_t mHandle = 0;
    };
}

#endif