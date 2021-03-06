#pragma once

namespace fan
{
    namespace impl
    {
        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr double FixedMinPositiveFractional( const int _fractionalSize )
        {
            double   val = 1.;
            for( int i   = 0; i < _fractionalSize; ++i ){ val /= 2; }
            return val;
        }

        //==============================================================================================================================================================================================
        // converts part of a string to an integer
        //==============================================================================================================================================================================================
        constexpr int64_t AtoiLimited( const char* _begin, const char* _end )
        {
            bool isNegative = _begin[0] == '-';
            if( isNegative ){ _begin++; }
            int64_t val = 0;
            if( _end == nullptr )
            {
                while( *_begin != '\0' )
                {
                    val = val * 10 + ( *_begin++ - '0' );
                }
            }
            else
            {
                while( _begin != _end )
                {
                    val = val * 10 + ( *_begin++ - '0' );
                }
            }
            return isNegative ? -val : val;
        }

        static constexpr bool IsValidNumberString( const char* _str )
        {
            const char* iterator = _str;

            // check first letter is 0123456789+-.
            char firstChar = *iterator++;
            if( ( firstChar < '0' || firstChar > '9' ) && firstChar != '.' && firstChar != '+' && firstChar != '-' )
            {
                return false;
            }

            // check other letters are 0123456789.
            while( *iterator != '\0' )
            {
                char c = *iterator++;
                if( ( c < '0' || c > '9' ) && c != '.' )
                {
                    return false;
                }
            }

            // check that there is only one '.'
            iterator = _str;
            int numDots = 0;
            while( *iterator != '\0' )
            {
                char c = *iterator++;
                if( c == '.' )
                {
                    numDots++;
                }
            }
            if( numDots > 1 )
            {
                return false;
            }

            return true;
        }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr uint64_t Pow10( uint64_t _pow )
        {
            return ( _pow >= sizeof( uint64_t ) * 8 ) ? 0 : _pow == 0 ? 1 : 10 * Pow10( _pow - 1 );
        }
        static_assert( Pow10( 2 ) == 100 );
        static_assert( Pow10( 3 ) == 1000 );
        static_assert( Pow10( 4 ) == 10000 );

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr int Factorial( const int _value )
        {
            int      value = 1;
            for( int i     = 2; i <= _value; ++i )
            {
                value *= i;
            }
            return value;
        }
        static_assert( Factorial( 2 ) == 2 );
        static_assert( Factorial( 3 ) == 6 );
        static_assert( Factorial( 4 ) == 24 );

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr const char* Strchr( const char* _begin, char _char )
        {
            while( *_begin != '\0' )
            {
                if( *_begin == _char )
                {
                    return _begin;
                }
                ++_begin;
            }
            return nullptr;
        }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr int64_t Min( int64_t _x, int64_t _y ) { return _x < _y ? _x : _y; }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr int64_t Strlen( const char* _begin )
        {
            int64_t len = 0;
            while( *_begin++ != '\0' )
            {
                ++len;
            }
            return len;
        }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        constexpr int32_t StringToFixed( const char* _str, const int32_t _fractionalSize, const int32_t _fractionalMask, const int32_t _fixedOne )
        {
            const bool isNegative = _str[0] == '-';
            const char* dotStr = Strchr( _str, '.' );
            const int64_t  integerValue       = AtoiLimited( _str, dotStr );
            const int64_t  maxDigits          = 8; // we clamp the maximum fractional digits to avoid overflow later (fractionalValue0)
            const int64_t  numDigits          = Min( dotStr != nullptr ? Strlen( dotStr + 1 ) : 0, maxDigits );
            const int64_t  rawFractionalValue = dotStr != nullptr ? AtoiLimited( dotStr + 1, dotStr + numDigits + 1 ) : 0;
            const uint64_t pow                = Pow10( numDigits );
            const uint64_t fractionalValue0   = uint64_t( _fixedOne ) * uint64_t( rawFractionalValue ); // this can overflow
            const int64_t  fractionalValue    = fractionalValue0 / pow;

            if( isNegative && fractionalValue > 0 )
            {
                int64_t t    = integerValue << _fractionalSize;
                int32_t data = static_cast<int32_t>(( t - _fixedOne ));
                data |= ( _fixedOne - fractionalValue ) & _fractionalMask;
                return data;
            }
            else
            {
                int32_t data = static_cast<int32_t>(integerValue << _fractionalSize);
                data |= static_cast<int32_t>(fractionalValue);
                return data;
            }
        }
    }
}