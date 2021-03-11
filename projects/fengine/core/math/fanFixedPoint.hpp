#pragma once

#include <stdint.h>
#include "core/math/fanFixedPointImpl.hpp"

namespace fan
{
    //==========================================================================================================================
    // reference : http://www.sunshine2k.de/articles/coding/fp/sunfp.html#ch51
    //==========================================================================================================================
    class Fixed
    {
    public:
        using DataType = int32_t;
        static constexpr int sIntegerSize    = 16;
        static constexpr int sFractionalSize = 16;
        static constexpr uint64_t sFixed_One      = 1 << sFractionalSize;
        static constexpr uint64_t sFractionalMask = sFixed_One - 1;

    public:
        static constexpr int32_t sMaxInteger    = ( 1 << ( sIntegerSize - 1 ) ) - 1;
        static constexpr int32_t sMinInteger    = -sMaxInteger - 1;
        static constexpr double  sMinFractional = impl::FixedMinPositiveFractional( sFractionalSize );
        static constexpr double  sMaxFractional = 1. - sMinFractional;
        static constexpr double  sMax           = sMaxInteger + sMaxFractional;
        static constexpr double  sMin           = sMinInteger;


        constexpr Fixed() : mData(0){}
        constexpr explicit Fixed( const int _integer ) :    mData(static_cast<DataType>(_integer << sFractionalSize)){}
        constexpr explicit Fixed( const float _float ) :    mData(static_cast<DataType>( _float * sFixed_One )){}
        constexpr explicit Fixed( const double _double ) :  mData(static_cast<DataType>( _double * sFixed_One )){}
        constexpr explicit Fixed( const char* _str ) :      mData( impl::StringToFixed(_str, sFractionalSize, sFractionalMask,sFixed_One) ){}

        static constexpr Fixed CreateFromData( const DataType _data ) { Fixed f; f.mData = _data; return f;}
        DataType GetData() const { return mData; }

        int ToInt() const       { return DataType( mData >> sFractionalSize ); }
        float ToFloat() const
        {
            return float( mData ) / sFixed_One;
        }
        double ToDouble() const { return double( mData ) / sFixed_One; }

        constexpr Fixed  operator+  ( const Fixed& _value ) const { return CreateFromData( mData + _value.mData ); }
        constexpr Fixed& operator+= ( const Fixed &_value ){ mData += _value.mData;  return *this; }
        constexpr Fixed& operator++ () { mData += sFixed_One; return *this;} // prefix ++
        constexpr Fixed  operator++ (int) { Fixed result = *this; ++(*this); return result; }// postfix ++

        constexpr Fixed  operator-  ( const Fixed& _value ) const{ return CreateFromData( mData - _value.mData ); }
        constexpr Fixed& operator-= ( const Fixed &_value ){ mData -= _value.mData;  return *this; }
        constexpr Fixed  operator-  () const { return CreateFromData( -mData );  } // unary -
        constexpr Fixed& operator-- () { mData -= sFixed_One; return *this;} // prefix --
        constexpr Fixed  operator-- (int) { Fixed result = *this; --(*this); return result; }// postfix --

        constexpr Fixed  operator*  ( const Fixed& _value ) const
        {
            Fixed result;
            result.mData = DataType( int64_t( mData ) * int64_t( _value.mData ) / sFixed_One );
            return result;
        }
        constexpr Fixed& operator*= ( const Fixed &_value ){  *this = *this * _value;  return *this; }

        constexpr Fixed  operator/  ( const Fixed& _value ) const
        {

                Fixed result;
                result.mData = DataType( ( int64_t( mData ) << sFractionalSize ) / _value.mData );
                return result;

        }
        constexpr Fixed& operator/= ( const Fixed &_value ){  *this = *this / _value;  return *this; }

        constexpr bool operator==( const Fixed& _value ) const{ return mData == _value.mData; }
        constexpr bool operator!=( const Fixed& _value ) const{ return mData != _value.mData; }

    private:
        DataType mData;
    };
    static_assert( sizeof( Fixed ) == 4 );

    constexpr Fixed operator "" _fx( const char* _string ){ return Fixed( _string ); }
}