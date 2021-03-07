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
    //==========================================================================================================================
    class Fixed
    {
    public:
        Fixed( const int _integer );
        Fixed( const float _float );
        Fixed( const double _double );
        Fixed( const char* _str );

        int ToInt() const;
        float ToFloat() const;
        double ToDouble() const;

    private:
        using DataType = int32_t;
        static constexpr int sIntegerSize    = 18;
        static constexpr int sFractionalSize = 14;

        static constexpr uint64_t sFixed_One      = 1 << sFractionalSize;
        static constexpr uint64_t sFractionalMask = sFixed_One - 1;
        static int AtoiLimited( const char* _begin, const char* _end );

        DataType mData;

    public:
        static constexpr int    sMaxInteger    = ( 1 << ( sIntegerSize - 1 ) ) - 1;
        static constexpr int    sMinInteger    = -sMaxInteger - 1;
        static constexpr double sMinFractional = impl::FixedMinPositiveFractional( sFractionalSize );
        static constexpr double sMaxFractional = 1. - sMinFractional;
        static constexpr double sMax           = sMaxInteger + sMaxFractional;
        static constexpr double sMin           = sMinInteger;
    };
    static_assert( sizeof( Fixed ) == 4 );

    Fixed operator "" _fx( const char* _string );
}