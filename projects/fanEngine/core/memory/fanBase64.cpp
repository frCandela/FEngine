#include "core/memory/fanBase64.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    static const std::string sBase64Chars =
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz"
                                     "0123456789+/";

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    static inline bool IsBase64( unsigned char _char )
    {
        return ( isalnum( _char ) || ( _char == '+' ) || ( _char == '/' ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Base64::Encode( const unsigned char* _bytesToEncode, const unsigned int _lenght )
    {
        std::string   encodedString;
        int           i           = 0;
        int           j           = 0;
        unsigned char charArray3[3];
        unsigned char charArray4[4];
        unsigned int  inputLenght = _lenght;

        while( inputLenght-- )
        {
            charArray3[i++] = *( _bytesToEncode++ );
            if( i == 3 )
            {
                charArray4[0] = ( charArray3[0] & 0xfc ) >> 2;
                charArray4[1] = ( ( charArray3[0] & 0x03 ) << 4 ) + ( ( charArray3[1] & 0xf0 ) >> 4 );
                charArray4[2] = ( ( charArray3[1] & 0x0f ) << 2 ) + ( ( charArray3[2] & 0xc0 ) >> 6 );
                charArray4[3] = charArray3[2] & 0x3f;

                for( i = 0; i < 4; i++ )
                {
                    encodedString += sBase64Chars[charArray4[i]];
                }
                i      = 0;
            }
        }

        if( i )
        {
            for( j = i; j < 3; j++ )
            {
                charArray3[j] = '\0';
            }

            charArray4[0] = ( charArray3[0] & 0xfc ) >> 2;
            charArray4[1] = ( ( charArray3[0] & 0x03 ) << 4 ) + ( ( charArray3[1] & 0xf0 ) >> 4 );
            charArray4[2] = ( ( charArray3[1] & 0x0f ) << 2 ) + ( ( charArray3[2] & 0xc0 ) >> 6 );

            for( j = 0; ( j < i + 1 ); j++ )
            {
                encodedString += sBase64Chars[charArray4[j]];
            }

            while( ( i++ < 3 ) )
            {
                encodedString += '=';
            }
        }

        return encodedString;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::string Base64::Decode( const std::string& _encodedString )
    {
        size_t        inputLenght = _encodedString.size();
        int           i           = 0;
        int           j           = 0;
        int           in_         = 0;
        unsigned char charArray4[4], charArray3[3];
        std::string   decodedString;

        while( inputLenght-- && ( _encodedString[in_] != '=' ) && IsBase64( _encodedString[in_] ) )
        {
            charArray4[i++] = _encodedString[in_];
            in_++;
            if( i == 4 )
            {
                for( i = 0; i < 4; i++ )
                {
                    charArray4[i] = sBase64Chars.find( charArray4[i] ) & 0xff;
                }

                charArray3[0] = ( charArray4[0] << 2 ) + ( ( charArray4[1] & 0x30 ) >> 4 );
                charArray3[1] = ( ( charArray4[1] & 0xf ) << 4 ) + ( ( charArray4[2] & 0x3c ) >> 2 );
                charArray3[2] = ( ( charArray4[2] & 0x3 ) << 6 ) + charArray4[3];

                for( i = 0; ( i < 3 ); i++ )
                {
                    decodedString += charArray3[i];
                }
                i      = 0;
            }
        }

        if( i )
        {
            for( j = 0; j < i; j++ )
            {
                charArray4[j] = sBase64Chars.find( charArray4[j] ) & 0xff;
            }

            charArray3[0] = ( charArray4[0] << 2 ) + ( ( charArray4[1] & 0x30 ) >> 4 );
            charArray3[1] = ( ( charArray4[1] & 0xf ) << 4 ) + ( ( charArray4[2] & 0x3c ) >> 2 );

            for( j = 0; j < i - 1; j++ )
            {
                decodedString += charArray3[j];
            }
        }

        return decodedString;
    }
}