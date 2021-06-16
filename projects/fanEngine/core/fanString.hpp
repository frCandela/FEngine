#pragma once

#include <string>
#include "fanJson.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // non allocating fixed size string
    //==================================================================================================================================================================================================
    template< int _BufferSize >
    struct String
    {
        static constexpr const int sCapacity = _BufferSize - 1;
        char                       mData[_BufferSize];
        int                        mSize;

        String();
        String( const char* _str );
        String( const std::string& _str );
        String( const Json& _json ) : String( static_cast<std::string>(_json) ) {}
        bool Empty() const { return mSize == 0; }

        operator const char*() const { return mData; }
        bool operator==( const char* _str ) const { return strcmp( _str, mData ) == 0; }
    };
}
#include "core/fanString.inl"

