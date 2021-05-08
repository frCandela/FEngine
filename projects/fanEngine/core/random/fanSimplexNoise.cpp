#include "core/random/fanSimplexNoise.hpp"
#include <random>

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SimplexNoise::SimplexNoise( const int _seed )
    {
        std::default_random_engine random;
        random.seed( _seed );

        // Fisher–Yates shuffle
        short shuffledPerms[256];
        std::memcpy( shuffledPerms, sPerms, 256 * sizeof( short ) );
        for( int i = 0; i < 256 - 1; i++ )
        {
            int r = random() % ( 256 - i );
            int j = i + r;
            std::swap( shuffledPerms[i], shuffledPerms[j] );
        }

        for( int i = 0; i < 512; i++ )
        {
            mPerms512[i]   = shuffledPerms[i & 255];
            mPermsMod12[i] = (short)( mPerms512[i] % 12 );
        }
    }

    //==================================================================================================================================================================================================
    // 2D simplex noise
    //==================================================================================================================================================================================================
    Fixed SimplexNoise::Noise( const Fixed _x, const Fixed _y ) const
    {
        Fixed n0, n1, n2; // Noise contributions from the three corners
        // Skew the input space to determine which simplex cell we're in
        Fixed s  = ( _x + _y ) * F2; // Hairy factor for 2D
        int   i  = Fixed::Floor( _x + s ).ToInt();
        int   j  = Fixed::Floor( _y + s ).ToInt();
        Fixed t  = ( i + j ) * G2;
        Fixed X0 = i - t; // Unskew the cell origin back to (x,y) space
        Fixed Y0 = j - t;
        Fixed x0 = _x - X0; // The x,y distances from the cell origin
        Fixed y0 = _y - Y0;
        // For the 2D case, the simplex shape is an equilateral triangle.
        // Determine which simplex we are in.
        int   i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
        if( x0 > y0 )
        {
            i1 = 1;
            j1 = 0;
        } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        else
        {
            i1 = 0;
            j1 = 1;
        }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
        // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
        // c = (3-sqrt(3))/6
        Fixed x1  = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
        Fixed y1  = y0 - j1 + G2;
        Fixed x2  = x0 - 1 + 2 * G2; // Offsets for last corner in (x,y) unskewed coords
        Fixed y2  = y0 - 1 + 2 * G2;
        // Work out the hashed gradient indices of the three simplex corners
        int   ii  = i & 255;
        int   jj  = j & 255;
        int   gi0 = mPermsMod12[ii + mPerms512[jj]];
        int   gi1 = mPermsMod12[ii + i1 + mPerms512[jj + j1]];
        int   gi2 = mPermsMod12[ii + 1 + mPerms512[jj + 1]];
        // Calculate the contribution from the three corners
        Fixed t0  = FIXED( 0.5 ) - x0 * x0 - y0 * y0;
        if( t0 < 0 ){ n0 = 0; }
        else
        {
            t0 *= t0;
            n0 = t0 * t0 * Vector3::Dot( sGrad3[gi0], { x0, y0, 0 } );  // (x,y) of grad3 used for 2D gradient
        }
        Fixed t1 = FIXED( 0.5 ) - x1 * x1 - y1 * y1;
        if( t1 < 0 ){ n1 = 0; }
        else
        {
            t1 *= t1;
            n1 = t1 * t1 * Vector3::Dot( sGrad3[gi1], { x1, y1, 0 } );
        }
        Fixed t2 = FIXED( 0.5 ) - x2 * x2 - y2 * y2;
        if( t2 < 0 ){ n2 = 0; }
        else
        {
            t2 *= t2;
            n2 = t2 * t2 * Vector3::Dot( sGrad3[gi2], { x2, y2, 0 } );
        }
        // Add contributions from each corner to get the final noise value.
        // The result is scaled to return values in the interval [-1,1].
        return 70 * ( n0 + n1 + n2 );
    }

    //==================================================================================================================================================================================================
    // 3D simplex noise
    //==================================================================================================================================================================================================
    Fixed SimplexNoise::Noise( const Vector3& _pos ) const
    {
        Fixed n0, n1, n2, n3; // Noise contributions from the four corners
        // Skew the input space to determine which simplex cell we're in
        Fixed s  = ( _pos.x + _pos.y + _pos.z ) * F3; // Very nice and simple skew factor for 3D
        int   i  = Fixed::Floor( _pos.x + s ).ToInt();
        int   j  = Fixed::Floor( _pos.y + s ).ToInt();
        int   k  = Fixed::Floor( _pos.z + s ).ToInt();
        Fixed t  = ( i + j + k ) * G3;
        Fixed X0 = i - t; // Unskew the cell origin back to (x,y,z) space
        Fixed Y0 = j - t;
        Fixed Z0 = k - t;
        Fixed x0 = _pos.x - X0; // The x,y,z distances from the cell origin
        Fixed y0 = _pos.y - Y0;
        Fixed z0 = _pos.z - Z0;
        // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
        // Determine which simplex we are in.
        int   i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
        int   i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
        if( x0 >= y0 )
        {
            if( y0 >= z0 )
            {
                i1 = 1;
                j1 = 0;
                k1 = 0;
                i2 = 1;
                j2 = 1;
                k2 = 0;
            } // X Y Z order
            else if( x0 >= z0 )
            {
                i1 = 1;
                j1 = 0;
                k1 = 0;
                i2 = 1;
                j2 = 0;
                k2 = 1;
            } // X Z Y order
            else
            {
                i1 = 0;
                j1 = 0;
                k1 = 1;
                i2 = 1;
                j2 = 0;
                k2 = 1;
            } // Z X Y order
        }
        else
        { // x0<y0
            if( y0 < z0 )
            {
                i1 = 0;
                j1 = 0;
                k1 = 1;
                i2 = 0;
                j2 = 1;
                k2 = 1;
            } // Z Y X order
            else if( x0 < z0 )
            {
                i1 = 0;
                j1 = 1;
                k1 = 0;
                i2 = 0;
                j2 = 1;
                k2 = 1;
            } // Y Z X order
            else
            {
                i1 = 0;
                j1 = 1;
                k1 = 0;
                i2 = 1;
                j2 = 1;
                k2 = 0;
            } // Y X Z order
        }
// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
// c = 1/6.
        Fixed x1  = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
        Fixed y1  = y0 - j1 + G3;
        Fixed z1  = z0 - k1 + G3;
        Fixed x2  = x0 - i2 + 2 * G3; // Offsets for third corner in (x,y,z) coords
        Fixed y2  = y0 - j2 + 2 * G3;
        Fixed z2  = z0 - k2 + 2 * G3;
        Fixed x3  = x0 - 1 + 3 * G3; // Offsets for last corner in (x,y,z) coords
        Fixed y3  = y0 - 1 + 3 * G3;
        Fixed z3  = z0 - 1 + 3 * G3;
// Work out the hashed gradient indices of the four simplex corners
        int   ii  = i & 255;
        int   jj  = j & 255;
        int   kk  = k & 255;
        int   gi0 = mPermsMod12[ii + mPerms512[jj + mPerms512[kk]]];
        int   gi1 = mPermsMod12[ii + i1 + mPerms512[jj + j1 + mPerms512[kk + k1]]];
        int   gi2 = mPermsMod12[ii + i2 + mPerms512[jj + j2 + mPerms512[kk + k2]]];
        int   gi3 = mPermsMod12[ii + 1 + mPerms512[jj + 1 + mPerms512[kk + 1]]];
// Calculate the contribution from the four corners
        Fixed t0  = FIXED( 0.6 ) - x0 * x0 - y0 * y0 - z0 * z0;
        if( t0 < 0 )
        {
            n0 = 0;
        }
        else
        {
            t0 *=
                    t0;
            n0 = t0 * t0 * Vector3::Dot( sGrad3[gi0], Vector3( x0, y0, z0 ) );
        }
        Fixed t1 = FIXED( 0.6 ) - x1 * x1 - y1 * y1 - z1 * z1;
        if( t1 < 0 )
        {
            n1 = 0;
        }
        else
        {
            t1 *=
                    t1;
            n1 = t1 * t1 * Vector3::Dot( sGrad3[gi1], Vector3( x1, y1, z1 ) );
        }
        Fixed t2 = FIXED( 0.6 ) - x2 * x2 - y2 * y2 - z2 * z2;
        if( t2 < 0 )
        {
            n2 = 0;
        }
        else
        {
            t2 *=
                    t2;
            n2 = t2 * t2 * Vector3::Dot( sGrad3[gi2], Vector3( x2, y2, z2 ) );
        }
        Fixed t3 = FIXED( 0.6 ) - x3 * x3 - y3 * y3 - z3 * z3;
        if( t3 < 0 )
        {
            n3 = 0;
        }
        else
        {
            t3 *=
                    t3;
            n3 = t3 * t3 * Vector3::Dot( sGrad3[gi3], Vector3( x3, y3, z3 ) );
        }
// Add contributions from each corner to get the final noise value.
// The result is scaled to stay just inside [-1,1]
        return 32 * ( n0 + n1 + n2 + n3 );
    }

    //==================================================================================================================================================================================================
    // 4 dimensional dot product
    //==================================================================================================================================================================================================
    Fixed SimplexNoise::Dot( const Grad4& _grad4, Fixed _x2, Fixed _y2, Fixed _z2, Fixed _w2 )
    {
        return _grad4.x * _x2 + _grad4.y * _y2 + _grad4.z * _z2 + _grad4.w * _w2;
    }

    //==================================================================================================================================================================================================
    // 4D simplex noise
    //==================================================================================================================================================================================================
    Fixed SimplexNoise::Noise( const Fixed _x, const Fixed _y, const Fixed _z, const Fixed _w ) const
    {

        Fixed n0, n1, n2, n3, n4; // Noise contributions from the five corners
        // Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
        Fixed s     = ( _x + _y + _z + _w ) * F4; // Factor for 4D skewing
        int   i     = Fixed::Floor( _x + s ).ToInt();
        int   j     = Fixed::Floor( _y + s ).ToInt();
        int   k     = Fixed::Floor( _z + s ).ToInt();
        int   l     = Fixed::Floor( _w + s ).ToInt();
        Fixed t     = ( i + j + k + l ) * G4; // Factor for 4D unskewing
        Fixed X0    = i - t; // Unskew the cell origin back to (x,y,z,w) space
        Fixed Y0    = j - t;
        Fixed Z0    = k - t;
        Fixed W0    = l - t;
        Fixed x0    = _x - X0;  // The x,y,z,w distances from the cell origin
        Fixed y0    = _y - Y0;
        Fixed z0    = _z - Z0;
        Fixed w0    = _w - W0;
        // For the 4D case, the simplex is a 4D shape I won't even try to describe.
        // To find out which of the 24 possible simplices we're in, we need to
        // determine the magnitude ordering of x0, y0, z0 and w0.
        // Six pair-wise comparisons are performed between each possible pair
        // of the four coordinates, and the results are used to rank the numbers.
        int   rankx = 0;
        int   ranky = 0;
        int   rankz = 0;
        int   rankw = 0;
        if( x0 > y0 ){ rankx++; }else{ ranky++; }
        if( x0 > z0 ){ rankx++; }else{ rankz++; }
        if( x0 > w0 ){ rankx++; }else{ rankw++; }
        if( y0 > z0 ){ ranky++; }else{ rankz++; }
        if( y0 > w0 ){ ranky++; }else{ rankw++; }
        if( z0 > w0 ){ rankz++; }else{ rankw++; }
        int i1, j1, k1, l1; // The integer offsets for the second simplex corner
        int i2, j2, k2, l2; // The integer offsets for the third simplex corner
        int i3, j3, k3, l3; // The integer offsets for the fourth simplex corner
        // [rankx, ranky, rankz, rankw] is a 4-vector with the numbers 0, 1, 2 and 3
        // in some order. We use a thresholding to set the coordinates in turn.
        // Rank 3 denotes the largest coordinate.
        i1 = rankx >= 3 ? 1 : 0;
        j1 = ranky >= 3 ? 1 : 0;
        k1 = rankz >= 3 ? 1 : 0;
        l1 = rankw >= 3 ? 1 : 0;
        // Rank 2 denotes the second largest coordinate.
        i2 = rankx >= 2 ? 1 : 0;
        j2 = ranky >= 2 ? 1 : 0;
        k2 = rankz >= 2 ? 1 : 0;
        l2 = rankw >= 2 ? 1 : 0;
        // Rank 1 denotes the second smallest coordinate.
        i3 = rankx >= 1 ? 1 : 0;
        j3 = ranky >= 1 ? 1 : 0;
        k3 = rankz >= 1 ? 1 : 0;
        l3 = rankw >= 1 ? 1 : 0;
        // The fifth corner has all coordinate offsets = 1, so no need to compute that.
        Fixed x1  = x0 - i1 + G4; // Offsets for second corner in (x,y,z,w) coords
        Fixed y1  = y0 - j1 + G4;
        Fixed z1  = z0 - k1 + G4;
        Fixed w1  = w0 - l1 + G4;
        Fixed x2  = x0 - i2 + 2 * G4; // Offsets for third corner in (x,y,z,w) coords
        Fixed y2  = y0 - j2 + 2 * G4;
        Fixed z2  = z0 - k2 + 2 * G4;
        Fixed w2  = w0 - l2 + 2 * G4;
        Fixed x3  = x0 - i3 + 3 * G4; // Offsets for fourth corner in (x,y,z,w) coords
        Fixed y3  = y0 - j3 + 3 * G4;
        Fixed z3  = z0 - k3 + 3 * G4;
        Fixed w3  = w0 - l3 + 3 * G4;
        Fixed x4  = x0 - 1 + 4 * G4; // Offsets for last corner in (x,y,z,w) coords
        Fixed y4  = y0 - 1 + 4 * G4;
        Fixed z4  = z0 - 1 + 4 * G4;
        Fixed w4  = w0 - 1 + 4 * G4;
        // Work out the hashed gradient indices of the five simplex corners
        int   ii  = i & 255;
        int   jj  = j & 255;
        int   kk  = k & 255;
        int   ll  = l & 255;
        int   gi0 = mPerms512[ii + mPerms512[jj + mPerms512[kk + mPerms512[ll]]]] % 32;
        int   gi1 = mPerms512[ii + i1 + mPerms512[jj + j1 + mPerms512[kk + k1 + mPerms512[ll + l1]]]] % 32;
        int   gi2 = mPerms512[ii + i2 + mPerms512[jj + j2 + mPerms512[kk + k2 + mPerms512[ll + l2]]]] % 32;
        int   gi3 = mPerms512[ii + i3 + mPerms512[jj + j3 + mPerms512[kk + k3 + mPerms512[ll + l3]]]] % 32;
        int   gi4 = mPerms512[ii + 1 + mPerms512[jj + 1 + mPerms512[kk + 1 + mPerms512[ll + 1]]]] % 32;
        // Calculate the contribution from the five corners
        Fixed t0  = FIXED( 0.6 ) - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
        if( t0 < 0 ){ n0 = 0; }
        else
        {
            t0 *= t0;
            n0 = t0 * t0 * Dot( sGrad4[gi0], x0, y0, z0, w0 );
        }
        Fixed t1 = FIXED( 0.6 ) - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
        if( t1 < 0 ){ n1 = 0; }
        else
        {
            t1 *= t1;
            n1 = t1 * t1 * Dot( sGrad4[gi1], x1, y1, z1, w1 );
        }
        Fixed t2 = FIXED( 0.6 ) - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
        if( t2 < 0 ){ n2 = 0; }
        else
        {
            t2 *= t2;
            n2 = t2 * t2 * Dot( sGrad4[gi2], x2, y2, z2, w2 );
        }
        Fixed t3 = FIXED( 0.6 ) - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
        if( t3 < 0 ){ n3 = 0; }
        else
        {
            t3 *= t3;
            n3 = t3 * t3 * Dot( sGrad4[gi3], x3, y3, z3, w3 );
        }
        Fixed t4 = FIXED( 0.6 ) - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
        if( t4 < 0 ){ n4 = 0; }
        else
        {
            t4 *= t4;
            n4 = t4 * t4 * Dot( sGrad4[gi4], x4, y4, z4, w4 );
        }
        // Sum up and scale the result to cover the range [-1,1]
        return 27 * ( n0 + n1 + n2 + n3 + n4 );
    }
}