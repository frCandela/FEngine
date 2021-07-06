#pragma once

#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include "core/fanPath.hpp"
#include "platform/fanSystem.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // runs a threads that watches files on the disk.
    // use GetFilesChanged() to safely retrieve the list of files that were changes.
    //==================================================================================================================================================================================================
    struct FilesWatcher
    {
        std::thread              mThread;
        bool                     mEnabled = false;
        std::vector<std::string> mOutFilesChanged;
        std::mutex               mMutex;

        void Start( const std::string& _path )
        {
            fanAssert( !mThread.joinable() && !mEnabled );
            mEnabled = true;
            mThread  = std::thread( System::WatchDirectory, _path, &mEnabled, &mOutFilesChanged, &mMutex );
        }

        void Stop()
        {
            fanAssert( mThread.joinable() && mEnabled );
            mEnabled = false;
            mThread.join();
        };

        std::vector<std::string> GetFilesChanged()
        {
            mMutex.lock();
            std::vector<std::string> copy = mOutFilesChanged;
            mOutFilesChanged.clear();
            mMutex.unlock();
            return copy;
        }
    };
}