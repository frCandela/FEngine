#pragma once

#include <stdint.h>
#include "core/math/fanFixedPointImpl.hpp"
#include "core/fanAssert.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // reference : http://www.sunshine2k.de/articles/coding/fp/sunfp.html#ch51
    //==================================================================================================================================================================================================
    struct Fixed
    {
    private:
        static constexpr int      sIntegerSize      = 16;
        static constexpr int      sFractionalSize   = 16;
        static constexpr uint64_t sFixed_One        = 1 << sFractionalSize;
        static constexpr uint64_t sFixed_Half       = 1 << ( sFractionalSize - 1 );
        static constexpr uint64_t sFractionalMask   = sFixed_One - 1;
        static constexpr uint64_t sIntegerMask      = ~sFractionalMask;
        static constexpr int      sSqrtMaxIteration = 10;
        static_assert( ( sFractionalMask & sIntegerMask ) == 0 );

        #define FX_TWO_PI       FIXED(6.283185307)
        #define FX_PI           FIXED(3.141592654)
        #define FX_HALF_PI      FIXED(1.570796327)
        #define FX_BIAS         FIXED(0.0001)
        #define FX_FUZZY_ZERO   FIXED(0.0005)
    public:
        using DataType = int32_t;
        static constexpr int32_t sMaxInteger    = ( 1 << ( sIntegerSize - 1 ) ) - 1;
        static constexpr int32_t sMinInteger    = -sMaxInteger - 1;
        static constexpr double  sMinFractional = impl::FixedMinPositiveFractional( sFractionalSize );
        static constexpr double  sMaxFractional = 1. - sMinFractional;
        static constexpr double  sMax           = sMaxInteger + sMaxFractional;
        static constexpr double  sMin           = sMinInteger;

        static const Fixed sMaxValue;
        static const Fixed sMinValue;
        static const Fixed sMinFractionalValue;
        static const Fixed sTwoPi;
        static const Fixed sPi;
        static const Fixed sHalfPi;
        static const Fixed sBias;
        static const Fixed sFuzzyZero;

        #define FIXED( str ) ([]() { static_assert( impl::IsValidNumberString(#str) ); constexpr Fixed x = Fixed(#str);   return x; }())

        constexpr Fixed() {}
        constexpr Fixed( const int _integer ) : mData( static_cast<DataType>(_integer << sFractionalSize) ) {}
        constexpr explicit Fixed( const char* _str ) : mData( impl::StringToFixed( _str, sFractionalSize, sFractionalMask, sFixed_One ) )
        {
            fanAssert( impl::IsValidNumberString( _str ) );
        }

        constexpr void SetData( const DataType _data ) { mData = _data; }

        static constexpr Fixed FromData( const DataType _data )
        {
            Fixed f;
            f.mData = _data;
            return f;
        }
        static constexpr Fixed FromFloat( const float _float )
        {
            Fixed f;
            f.mData = DataType( _float * sFixed_One );
            return f;
        }

        static constexpr Fixed FromDouble( const double _double )
        {
            Fixed f;
            f.mData = DataType( _double * sFixed_One );
            return f;
        }

        DataType& GetData() { return mData; }
        const DataType& GetData() const { return mData; }

        constexpr inline int ToInt() const { return DataType( mData >> sFractionalSize ); }
        constexpr inline float ToFloat() const { return float( mData ) / sFixed_One; }
        constexpr inline double ToDouble() const { return double( mData ) / sFixed_One; }

        constexpr bool operator==( const Fixed& _value ) const { return mData == _value.mData; }
        constexpr bool operator!=( const Fixed& _value ) const { return mData != _value.mData; }
        constexpr bool operator<( const Fixed& _value ) const { return mData < _value.mData; }
        constexpr bool operator<=( const Fixed& _value ) const { return mData <= _value.mData; }
        constexpr bool operator>( const Fixed& _value ) const { return mData > _value.mData; }
        constexpr bool operator>=( const Fixed& _value ) const { return mData >= _value.mData; }

        constexpr Fixed operator+( const Fixed& _value ) const
        {
            Fixed f;
            f.mData = mData + _value.mData;
            return f;;
        }
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

        constexpr Fixed operator-( const Fixed& _value ) const
        {
            Fixed f;
            f.mData = mData - _value.mData;
            return f;
        }
        constexpr Fixed& operator-=( const Fixed& _value )
        {
            mData -= _value.mData;
            return *this;
        }
        constexpr Fixed operator-() const// unary (-)
        {
            Fixed f;
            f.mData = -mData;
            return f;
        }
        constexpr Fixed operator+() const// unary (+)
        {
            return *this;
        }
        constexpr Fixed& operator--() // prefix (--)
        {
            mData -= sFixed_One;
            return *this;
        }

        constexpr Fixed operator--( int )// postfix (--)
        {
            Fixed result = *this;
            --( *this );
            return result;
        }

        constexpr Fixed operator*( const Fixed& _value ) const
        {
            Fixed result;
            result.mData = DataType( int64_t( mData ) * int64_t( _value.mData ) / int64_t( sFixed_One ) );
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

        constexpr bool IsFuzzyZero() const
        {
            return Abs( *this ) < FX_FUZZY_ZERO;
        }

        static constexpr bool IsFuzzyZero( const Fixed& _value )
        {
            return _value.IsFuzzyZero();
        }

        static constexpr Fixed Degrees( const Fixed& _radians )
        {
            return ( FIXED( 180 ) * _radians ) / FX_PI;
        }

        static constexpr Fixed Radians( const Fixed& _degrees )
        {
            return ( FX_PI * _degrees ) / FIXED( 180 );
        }

        static constexpr Fixed Sign( const Fixed& _value )
        {
            return _value < 0 ? -1 : 1;
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
        static constexpr Fixed Max( const Fixed& _a, const Fixed& _b ) { return _a > _b ? _a : _b; }
        static constexpr Fixed Min( const Fixed& _a, const Fixed& _b ) { return _a < _b ? _a : _b; }
        static constexpr Fixed PowI( const Fixed& _value, const int _power )
        {
            if( _power == 0 ){ return Fixed( 1 ); }

            Fixed    result = _value;
            for( int i      = 1; i < _power; ++i )
            {
                result *= _value;
            }
            return result;
        }
        static constexpr Fixed Sin( const Fixed& _value )
        {
            // clamp to [-PI;PI]
            Fixed clampedValue = _value % FX_TWO_PI;
            if( clampedValue > FX_PI ){ clampedValue -= FX_TWO_PI; }
            if( clampedValue < -FX_PI ){ clampedValue += FX_TWO_PI; }

            // use trigonometric identities to calculate on [-PI/2;PI/2]
            if( clampedValue + FX_BIAS < -FX_HALF_PI ){ return -Cos( clampedValue + FX_HALF_PI ); }
            if( clampedValue - FX_BIAS > FX_HALF_PI ){ return Cos( FX_HALF_PI - clampedValue ); }

            // return Taylor series approximation at zero: sin(x) = x - x^3/3! + x^5/5! - x^7/7! + x^9/9!
            Fixed tmp3 = PowI( clampedValue, 3 ) / Fixed( impl::Factorial( 3 ) );
            Fixed tmp5 = PowI( clampedValue, 5 ) / Fixed( impl::Factorial( 5 ) );
            Fixed tmp7 = PowI( clampedValue, 7 ) / Fixed( impl::Factorial( 7 ) );
            Fixed tmp9 = PowI( clampedValue, 9 ) / Fixed( impl::Factorial( 7 ) );
            tmp9 /= Fixed( 8 ) * Fixed( 9 ); // calculate in two parts to avoid overflow
            return clampedValue - tmp3 + tmp5 - tmp7 + tmp9;
        }
        static constexpr Fixed ASin( const Fixed& _value )
        {
            Fixed negate = _value < 0 ? 1 : 0;
            Fixed x      = Fixed::Abs( _value );
            Fixed ret    = -FIXED( 0.0187293 );
            ret *= x;
            ret += FIXED( 0.0742610 );
            ret *= x;
            ret -= FIXED( 0.2121144 );
            ret *= x;
            ret += FIXED( 1.5707288 );
            ret          = FX_PI * FIXED( 0.5 ) - Fixed::Sqrt( FIXED( 1 ) - x ) * ret;
            return ret - FIXED( 2 ) * negate * ret;
        }
        static constexpr Fixed ATan2( const Fixed& _y, const Fixed& _x )
        {
            Fixed t0, t1, t2, t3, t4;

            t3 = Fixed::Abs( _x );
            t1 = Fixed::Abs( _y );
            t0 = Fixed::Max( t3, t1 );
            t1 = Fixed::Min( t3, t1 );
            t3 = FIXED( 1 ) / t0;
            t3 = t1 * t3;

            t4 = t3 * t3;
            t0 = -FIXED( 0.013480470 );
            t0 = t0 * t4 + FIXED( 0.057477314 );
            t0 = t0 * t4 - FIXED( 0.121239071 );
            t0 = t0 * t4 + FIXED( 0.195635925 );
            t0 = t0 * t4 - FIXED( 0.332994597 );
            t0 = t0 * t4 + FIXED( 0.999995630 );
            t3 = t0 * t3;

            t3 = ( Fixed::Abs( _y ) > Fixed::Abs( _x ) ) ? FIXED( 1.570796327 ) - t3 : t3;
            t3 = ( _x < 0 ) ? FIXED( 3.141592654 ) - t3 : t3;
            t3 = ( _y < 0 ) ? -t3 : t3;

            return t3;
        }
        static constexpr Fixed Cos( const Fixed& _value )
        {
            // clamp to [-PI;PI]
            Fixed clampedValue = _value % FX_TWO_PI;
            if( clampedValue > FX_PI ){ clampedValue -= FX_TWO_PI; }
            if( clampedValue < -FX_PI ){ clampedValue += FX_TWO_PI; }

            // use trigonometric identities to calculate on [-PI/2;PI/2]
            if( clampedValue + FX_BIAS < -FX_HALF_PI ){ return Sin( clampedValue + FX_HALF_PI ); }
            if( clampedValue - FX_BIAS > FX_HALF_PI ){ return Sin( FX_HALF_PI - clampedValue ); }

            // return Taylor series approximation at zero: sin(x) = x - x^2/2! + x^4/4! - x^6/6! + x^8/8!
            Fixed tmp2 = PowI( clampedValue, 2 ) / Fixed( impl::Factorial( 2 ) );
            Fixed tmp4 = PowI( clampedValue, 4 ) / Fixed( impl::Factorial( 4 ) );
            Fixed tmp6 = PowI( clampedValue, 6 ) / Fixed( impl::Factorial( 6 ) );
            Fixed tmp8 = PowI( clampedValue, 8 ) / Fixed( impl::Factorial( 7 ) );
            tmp8 /= Fixed( 8 ); // calculate in two parts to avoid overflow
            return Fixed( 1 ) - tmp2 + tmp4 - tmp6 + tmp8;
        }
        static constexpr Fixed Tan( const Fixed& _value )
        {
            return Sin( _value ) / Cos( _value );
        }
        static constexpr Fixed ACos( const Fixed& _value )
        {
            // https://developer.download.nvidia.com/cg/acos.html
            // Handbook of Mathematical Functions
            // M. Abramowitz and I.A. Stegun, Ed.
            Fixed negate = _value < 0 ? 1 : 0;
            Fixed x      = Fixed::Abs( _value );
            Fixed ret    = FIXED( 0.0187293 );
            ret *= x;
            ret += FIXED( 0.0742610 );
            ret *= x;
            ret -= FIXED( 0.2121144 );
            ret *= x;
            ret += FIXED( 1.5707288 );
            ret *= Fixed::Sqrt( FIXED( 1.0 ) - x );
            ret -= FIXED( 2 ) * negate * ret;
            return negate * FIXED( 3.14159265358979 ) + ret;
        }

        static constexpr Fixed Sqrt( const Fixed& _value )
        {
            if( _value <= Fixed( 0 ) ){ return Fixed( 0 ); }

            // approximate sqrt(_value) using a linear estimate
            // @see https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
            Fixed pow( 0 );
            Fixed a      = _value;
            if( _value < Fixed( 1 ) )
            {
                a *= Fixed( 100 );
                pow = FIXED( 0.1 );
            }
            else
            {
                int n = 0;
                while( a >= Fixed( 100 ) )
                {
                    a /= Fixed( 100 );
                    n += 1;
                }
                pow   = Fixed::PowI( Fixed( 10 ), n );
            }
            Fixed approx = a < Fixed( 10 )
                    ? ( FIXED( 0.28 ) * a + FIXED( 0.89 ) ) * pow
                    : ( FIXED( 0.089 ) * a + FIXED( 2.8 ) ) * pow;

            // calculates sqrt using Newton's method
            Fixed result    = approx;
            int   iteration = 0;
            while( iteration++ < sSqrtMaxIteration )
            {
                result                   = ( result + ( _value / result ) ) / Fixed( 2 );
                bool maxPrecisionReached = Fixed::Abs( ( result * result ) - _value ) == Fixed( 0 );
                if( maxPrecisionReached )
                {
                    break;
                }
            }
            return result;
        }
        static Fixed InvSqrt( const Fixed& _value ) // faster but with less precision
        {
            Fixed pow    = FIXED( 0 );
            Fixed a      = _value;
            if( _value < Fixed( 1 ) )
            {
                a *= FIXED( 100 );
                pow = FIXED( 0.1 );
            }
            else
            {
                int                   n              = 0;
                constexpr const Fixed divideLookup[] = { 1, 100, 10000, 1000000 };
                while( a >= divideLookup[n + 1] )
                {
                    n += 1;
                }
                a /= divideLookup[n];
                constexpr const Fixed powLookup[] = { 1, 10, 100, 1000, 10000, 100000 };
                pow = powLookup[n];
            }
            Fixed approx = a < Fixed( 10 )
                    ? ( FIXED( 0.28 ) * a + FIXED( 0.89 ) ) * pow
                    : ( FIXED( 0.089 ) * a + FIXED( 2.8 ) ) * pow;

            a = FIXED( 1 ) / approx;
            a = a * ( FIXED( 1.5 ) - FIXED( 0.5 ) * _value * a * a );
            a = a * ( FIXED( 1.5 ) - FIXED( 0.5 ) * _value * a * a );
            return a;
        }
    private:
        DataType mData;
    };
    static_assert( sizeof( Fixed ) == 4 );

    constexpr inline Fixed operator*( const int _int, const Fixed& _value ) { return Fixed( _int ) * _value; }
    constexpr inline Fixed operator+( const int _int, const Fixed& _value ) { return Fixed( _int ) + _value; }
    constexpr inline Fixed operator-( const int _int, const Fixed& _value ) { return Fixed( _int ) - _value; }
    constexpr inline Fixed operator/( const int _int, const Fixed& _value ) { return Fixed( _int ) / _value; }
}