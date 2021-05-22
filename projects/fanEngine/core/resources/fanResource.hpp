#pragma once

#include "core/fanHash.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    #define FAN_RESOURCE( _ResourceType )                               \
    public:                                                             \
    template <class T> struct fanResourceInfo                           \
    {                                                                   \
        static constexpr const char* sName{ #_ResourceType };           \
        static constexpr uint32_t    sType{ SSID( #_ResourceType ) };   \
    };                                                                  \
    using Info = fanResourceInfo< _ResourceType >

    //==================================================================================================================================================================================================
    // Base class for resources ( Mesh, textures etc.)
    //==================================================================================================================================================================================================
    struct Resource
    {
        int         mRefCount = 0;
        uint32_t    mGUID;
        uint32_t    mType;
    };
}