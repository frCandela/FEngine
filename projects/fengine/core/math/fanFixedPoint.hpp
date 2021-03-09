#pragma once

#include <stdint.h>

namespace fan
{
    namespace impl
    {
        //======================================================================================================================
        //======================================================================================================================
        constexpr double FixedMinPositiveFractional( const int _fractionalSize )
        {
            double   val = 1.;
            for( int i   = 0; i < _fractionalSize; ++i ){ val /= 2; }
            return val;
        }
    }

    //==========================================================================================================================
    // reference : http://www.sunshine2k.de/articles/coding/fp/sunfp.html#ch51
    //==========================================================================================================================
    class Fixed
    {
    private:
        using DataType = int32_t;
        static constexpr int sIntegerSize    = 18;
        static constexpr int sFractionalSize = 14;

    public:
        static constexpr int    sMaxInteger    = ( 1 << ( sIntegerSize - 1 ) ) - 1;
        static constexpr int    sMinInteger    = -sMaxInteger - 1;
        static constexpr double sMinFractional = impl::FixedMinPositiveFractional( sFractionalSize );
        static constexpr double sMaxFractional = 1. - sMinFractional;
        static constexpr double sMax           = sMaxInteger + sMaxFractional;
        static constexpr double sMin           = sMinInteger;


        constexpr Fixed() : mData(0){}
        constexpr explicit Fixed( const int _integer ) : mData(static_cast<DataType>(_integer << sFractionalSize)){}
        constexpr explicit Fixed( const float _float ) : mData(static_cast<DataType>( _float * sFixed_One )){}
        constexpr explicit Fixed( const double _double ) : mData(static_cast<DataType>( _double * sFixed_One )){}
        explicit Fixed( const char* _str );

        static Fixed CreateFromData( const DataType _data ) { Fixed f; f.mData = _data; return f;}
        DataType GetData() const { return mData; }

        int ToInt() const;
        float ToFloat() const;
        double ToDouble() const;

        Fixed  operator+  ( const Fixed& _value ) const { return CreateFromData( mData + _value.mData ); }
        Fixed& operator+= ( const Fixed &_value ){ mData += _value.mData;  return *this; }
        Fixed& operator++ () { mData += sFixed_One; return *this;} // prefix ++
        Fixed  operator++ (int) { Fixed result = *this; ++(*this); return result; }// postfix ++

        Fixed  operator-  ( const Fixed& _value ) const{ return CreateFromData( mData - _value.mData ); }
        Fixed& operator-= ( const Fixed &_value ){ mData -= _value.mData;  return *this; }
        Fixed  operator-  () const { return CreateFromData( -mData );  } // unary -
        Fixed& operator-- () { mData -= sFixed_One; return *this;} // prefix --
        Fixed  operator-- (int) { Fixed result = *this; --(*this); return result; }// postfix --

        Fixed  operator*  ( const Fixed& _value ) const;
        Fixed& operator*= ( const Fixed &_value ){  *this = *this * _value;  return *this; }

        Fixed  operator/  ( const Fixed& _value ) const;
        Fixed& operator/= ( const Fixed &_value ){  *this = *this / _value;  return *this; }

        bool operator==( const Fixed& _value ) const{ return mData == _value.mData; }
        bool operator!=( const Fixed& _value ) const{ return mData != _value.mData; }

    private:
        static constexpr uint64_t sFixed_One      = 1 << sFractionalSize;
        static constexpr uint64_t sFractionalMask = sFixed_One - 1;
        static int AtoiLimited( const char* _begin, const char* _end );

        DataType mData;
    };
    static_assert( sizeof( Fixed ) == 4 );

    Fixed operator "" _fx( const char* _string );
}