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
    private:
        using DataType = int32_t;
        static constexpr int      sIntegerSize    = 16;
        static constexpr int      sFractionalSize = 16;
        static constexpr uint64_t sFixed_One      = 1 << sFractionalSize;
        static constexpr uint64_t sFixed_Half     = 1 << ( sFractionalSize - 1 );
        static constexpr uint64_t sFractionalMask = sFixed_One - 1;
        static constexpr uint64_t sIntegerMask    = ~sFractionalMask;
        static_assert( ( sFractionalMask & sIntegerMask ) == 0 );

    public:
        static constexpr int32_t sMaxInteger    = ( 1 << ( sIntegerSize - 1 ) ) - 1;
        static constexpr int32_t sMinInteger    = -sMaxInteger - 1;
        static constexpr double  sMinFractional = impl::FixedMinPositiveFractional( sFractionalSize );
        static constexpr double  sMaxFractional = 1. - sMinFractional;
        static constexpr double  sMax           = sMaxInteger + sMaxFractional;
        static constexpr double  sMin           = sMinInteger;

        #define FX_TWO_PI   Fixed("6.283185307")
        #define FX_PI       Fixed("3.141592654")
        #define FX_HALF_PI  Fixed("1.570796327")

        constexpr Fixed() : mData( 0 ) {}
        constexpr explicit Fixed( const int _integer ) : mData( static_cast<DataType>(_integer << sFractionalSize) ) {}
        constexpr explicit Fixed( const float _float ) : mData( static_cast<DataType>( _float * sFixed_One ) ) {}
        constexpr explicit Fixed( const double _double ) : mData( static_cast<DataType>( _double * sFixed_One ) ) {}
        constexpr explicit Fixed( const char* _str ) : mData( impl::StringToFixed( _str,
                                                                                   sFractionalSize,
                                                                                   sFractionalMask,
                                                                                   sFixed_One ) ) {}

        static constexpr Fixed CreateFromData( const DataType _data )
        {
            Fixed f;
            f.mData = _data;
            return f;
        }
        DataType GetData() const { return mData; }

        int ToInt() const { return DataType( mData >> sFractionalSize ); }
        float ToFloat() const { return float( mData ) / sFixed_One; }
        double ToDouble() const { return double( mData ) / sFixed_One; }

        constexpr bool operator==( const Fixed& _value ) const { return mData == _value.mData; }
        constexpr bool operator!=( const Fixed& _value ) const { return mData != _value.mData; }
        constexpr bool operator<( const Fixed& _value ) const { return mData < _value.mData; }
        constexpr bool operator<=( const Fixed& _value ) const { return mData <= _value.mData; }
        constexpr bool operator>( const Fixed& _value ) const { return mData > _value.mData; }
        constexpr bool operator>=( const Fixed& _value ) const { return mData >= _value.mData; }

        constexpr Fixed operator+( const Fixed& _value ) const { return CreateFromData( mData + _value.mData ); }
        constexpr Fixed& operator+=( const Fixed& _value )
        {
            mData += _value.mData;
            return *this;
        }
        constexpr Fixed& operator++()
        {
            mData += sFixed_One;
            return *this;
        } // prefix (++)
        constexpr Fixed operator++( int )
        {
            Fixed result = *this;
            ++( *this );
            return result;
        }// postfix (++)

        constexpr Fixed operator-( const Fixed& _value ) const { return CreateFromData( mData - _value.mData ); }
        constexpr Fixed& operator-=( const Fixed& _value )
        {
            mData -= _value.mData;
            return *this;
        }
        constexpr Fixed operator-() const { return CreateFromData( -mData ); } // unary (-)
        constexpr Fixed& operator--()
        {
            mData -= sFixed_One;
            return *this;
        } // prefix (--)
        constexpr Fixed operator--( int )
        {
            Fixed result = *this;
            --( *this );
            return result;
        }// postfix (--)

        constexpr Fixed operator*( const Fixed& _value ) const
        {
            Fixed result;
            result.mData = DataType( int64_t( mData ) * int64_t( _value.mData ) / sFixed_One );
            return result;
        }
        constexpr Fixed& operator*=( const Fixed& _value )
        {
            *this = *this * _value;
            return *this;
        }

        constexpr Fixed operator/( const Fixed& _value ) const
        {

            Fixed result;
            result.mData = DataType( ( int64_t( mData ) << sFractionalSize ) / _value.mData );
            return result;
        }
        constexpr Fixed& operator/=( const Fixed& _value )
        {
            *this = *this / _value;
            return *this;
        }

        constexpr Fixed operator%( const Fixed& _value ) const
        {
            if( _value == Fixed( 0 ) ){ return _value; }

            Fixed n = *this / _value;
            n = n > Fixed( 0 ) ? Floor( n ) : Ceil( n );
            return *this - n * _value;
        }
        constexpr Fixed& operator%=( const Fixed& _value )
        {
            *this = *this % _value;
            return *this;
        }

        static constexpr Fixed Floor( const Fixed& _value )
        {
            Fixed result;
            result.mData = _value.mData & sIntegerMask;
            return result;
        }
        static constexpr Fixed Ceil( const Fixed& _value )
        {
            if( ( _value.mData & sFractionalMask ) != 0 )
            {
                Fixed result;
                result.mData = ( ( _value.mData + sFixed_One ) & sIntegerMask );
                return result;
            }
            else
            {
                return _value;
            }
        }
        static constexpr Fixed Round( const Fixed& _value )
        {
            Fixed result;
            result.mData = _value.mData + sFixed_Half;
            return Floor( result );
        }
        static constexpr Fixed Abs( const Fixed& _value )
        {
            if( _value.mData < 0 )
            {
                Fixed result;
                result.mData = -_value.mData;
                return result;
            }
            else
            {
                return _value;
            }
        }
        static constexpr Fixed PowI( const Fixed& _value, const int _power )
        {
            Fixed    result = _value;
            for( int i      = 1; i < _power; ++i )
            {
                result *= _value;
            }
            return result;
        }
        static constexpr Fixed Sin( const Fixed& _value )
        {
            // return Taylor series approximation sin(x) = x - x^3/3! + x^5/5! - x^7/7! + x^9/9!
            Fixed clampedValue = _value % FX_TWO_PI;
            Fixed tmp3         = PowI( clampedValue, 3 ) / Fixed( impl::Factorial( 3 ) );
            Fixed tmp5         = PowI( clampedValue, 5 ) / Fixed( impl::Factorial( 5 ) );
            Fixed tmp7         = PowI( clampedValue, 7 ) / Fixed( impl::Factorial( 7 ) );
            Fixed tmp9         = PowI( clampedValue, 9 ) / Fixed( impl::Factorial( 7 ) );
            tmp9 /= Fixed( 8 ) * Fixed( 9 ); // calculate in two parts to avoid overflow
            return clampedValue - tmp3 + tmp5 - tmp7 + tmp9;
        }

    private:
        DataType mData;
    };
    static_assert( sizeof( Fixed ) == 4 );

    constexpr Fixed operator "" _fx( const char* _string ) { return Fixed( _string ); }
}