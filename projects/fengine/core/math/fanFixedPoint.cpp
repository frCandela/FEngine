#include <iostream>
#include <bitset>
#include "core/math/fanFixedPoint.hpp"
#include "core/fanAssert.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    Fixed::Fixed( const int _integer )
    {
        mData = static_cast<DataType>(_integer << sFractionalSize);
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed::Fixed( const float _float )
    {
        fanAssertMsg( false, "converting floating point to fixed point is not deterministic" );
        mData = static_cast<DataType>( _float * sFixed_One );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed::Fixed( const double _double )
    {
        fanAssertMsg( false, "converting floating point to fixed point is not deterministic" );
        mData = static_cast<DataType>( _double * sFixed_One );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed::Fixed( const char* _str )
    {
        const char* dotStr = strchr( _str, '.' );
        const int64_t  integerValue       = dotStr != nullptr ? AtoiLimited( _str, dotStr ) : _atoi64( _str );
        const uint64_t rawFractionalValue = dotStr != nullptr ? _atoi64( dotStr + 1 ) : 0;
        const int64_t  numDigits          = dotStr != nullptr ? (int)strlen( dotStr + 1 ) : 0;
        const uint64_t pow                = (uint64_t)std::pow( 10, numDigits );
        const uint64_t fractionalValue    = static_cast<uint64_t>( ( sFixed_One * rawFractionalValue ) / pow);
        if( integerValue < 0 )
        {
            mData = static_cast<DataType>(( integerValue << sFractionalSize ) - sFixed_One);
            mData |= ( sFixed_One - fractionalValue ) & sFractionalMask;
        }
        else
        {
            mData = static_cast<DataType>(integerValue << sFractionalSize);
            mData |= static_cast<DataType>(fractionalValue);
        }
    }

    //==========================================================================================================================
    //==========================================================================================================================
    int Fixed::ToInt() const
    {
        return DataType( mData >> sFractionalSize );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    float Fixed::ToFloat() const
    {
        return float( mData ) / sFixed_One;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    double Fixed::ToDouble() const
    {
        return double( mData ) / sFixed_One;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed operator "" _fx( const char* _str )
    {
        return Fixed( _str );
    }

    //==========================================================================================================================
    // converts part of a string to an integer
    //==========================================================================================================================
    int Fixed::AtoiLimited( const char* _begin, const char* _end )
    {
        fanAssert( _begin[0] != '+' );
        bool isNegative = _begin[0] == '-';
        if( isNegative ){ *_begin++; }
        int val = 0;
        while( _begin != _end )
        {
            val = val * 10 + ( *_begin++ - '0' );
        }
        return isNegative ? -val : val;
    }
}