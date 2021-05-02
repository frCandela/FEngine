#pragma once

#include "core/math/fanFixedPoint.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{

    //==================================================================================================================================================================================================
    // Based from Stefan Gustavson simplex noise algorithm
    //==================================================================================================================================================================================================
    struct SimplexNoise
    {
        SimplexNoise( int seed );
        Fixed Noise( const Fixed _x, const Fixed _y ) const;
        Fixed Noise( const Vector3& _pos ) const;
        Fixed Noise( const Fixed _x, const Fixed _y, const Fixed _z, const Fixed _w ) const;

    private:
        short mPerms512[512];// To remove the need for index wrapping, Fixed the permutation table length
        short mPermsMod12[512];

        static const constexpr short sPerms[256] = { 151, 160, 137, 91, 90, 15,
                                                     131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
                                                     190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
                                                     88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
                                                     77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
                                                     102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
                                                     135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
                                                     5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
                                                     223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
                                                     129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
                                                     251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
                                                     49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
                                                     138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };
        // Skewing and unskewing factors for 2 and 3 dimensions
        static const constexpr Fixed F2          = FIXED( 0.5 ) * ( Fixed::Sqrt( 3 ) - 1 );
        static const constexpr Fixed G2          = ( 3 - Fixed::Sqrt( 3 ) ) / 6;
        static const constexpr Fixed F3          = FIXED( 1 ) / 3;
        static const constexpr Fixed G3          = FIXED( 1 ) / 6;
        static const constexpr Fixed F4          = ( Fixed::Sqrt( 5 ) - 1 ) / 4;
        static const constexpr Fixed G4          = ( 5 - Fixed::Sqrt( 5 ) ) / 20;
        struct Grad4
        {
            Fixed x;
            Fixed y;
            Fixed z;
            Fixed w;
        };
        static Fixed Dot( const Grad4& _grad4, Fixed _x2, Fixed _y2, Fixed _z2, Fixed _w2 );

        static const constexpr Vector3 sGrad3[12] = { Vector3( 1, 1, 0 ), Vector3( -1, 1, 0 ), Vector3( 1, -1, 0 ), Vector3( -1, -1, 0 ),
                                                      Vector3( 1, 0, 1 ), Vector3( -1, 0, 1 ), Vector3( 1, 0, -1 ), Vector3( -1, 0, -1 ),
                                                      Vector3( 0, 1, 1 ), Vector3( 0, -1, 1 ), Vector3( 0, 1, -1 ), Vector3( 0, -1, -1 ) };
        static const constexpr Grad4   sGrad4[]   = { { 0,  1,  1,  1 },
                                                      { 0,  1,  1,  -1 },
                                                      { 0,  1,  -1, 1 },
                                                      { 0,  1,  -1, -1 },
                                                      { 0,  -1, 1,  1 },
                                                      { 0,  -1, 1,  -1 },
                                                      { 0,  -1, -1, 1 },
                                                      { 0,  -1, -1, -1 },
                                                      { 1,  0,  1,  1 },
                                                      { 1,  0,  1,  -1 },
                                                      { 1,  0,  -1, 1 },
                                                      { 1,  0,  -1, -1 },
                                                      { -1, 0,  1,  1 },
                                                      { -1, 0,  1,  -1 },
                                                      { -1, 0,  -1, 1 },
                                                      { -1, 0,  -1, -1 },
                                                      { 1,  1,  0,  1 },
                                                      { 1,  1,  0,  -1 },
                                                      { 1,  -1, 0,  1 },
                                                      { 1,  -1, 0,  -1 },
                                                      { -1, 1,  0,  1 },
                                                      { -1, 1,  0,  -1 },
                                                      { -1, -1, 0,  1 },
                                                      { -1, -1, 0,  -1 },
                                                      { 1,  1,  1,  0 },
                                                      { 1,  1,  -1, 0 },
                                                      { 1,  -1, 1,  0 },
                                                      { 1,  -1, -1, 0 },
                                                      { -1, 1,  1,  0 },
                                                      { -1, 1,  -1, 0 },
                                                      { -1, -1, 1,  0 },
                                                      { -1, -1, -1, 0 }, };
    };
}