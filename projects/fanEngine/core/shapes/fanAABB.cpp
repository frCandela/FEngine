#include "core/shapes/fanAABB.hpp"
#include "core/fanAssert.hpp"
#include "core/math/fanMatrix4.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    AABB::AABB() :
            mLow(),
            mHigh() {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    AABB::AABB( const Vector3 _low, const Vector3 _high ) :
            mLow( _low ),
            mHigh( _high )
    {
        fanAssert( _low.x <= _high.x && _low.y <= _high.y && _low.z <= _high.z );
    }

    //==================================================================================================================================================================================================
    // Computes the AABB of a a transformed points cloud
    //==================================================================================================================================================================================================
    AABB::AABB( const std::vector<Vector3>& _pointCloud, const Matrix4& _tranform )
    {
        if( _pointCloud.empty() )
        {
            mLow  = FIXED( -0.5 ) * Vector3::sOne;
            mHigh = FIXED( 0.5 ) * Vector3::sOne;
            return;
        }

        mLow  = Vector3( Fixed::sMaxValue, Fixed::sMaxValue, Fixed::sMaxValue );
        mHigh = Vector3( Fixed::sMinValue, Fixed::sMinValue, Fixed::sMinValue );
        for( int index = 0; index < (int)_pointCloud.size(); index++ )
        {
            const Vector3 vertex = _tranform * _pointCloud[index];
            if( vertex.x < mLow.x ){ mLow.x = vertex.x; }
            if( vertex.y < mLow.y ){ mLow.y = vertex.y; }
            if( vertex.z < mLow.z ){ mLow.z = vertex.z; }
            if( vertex.x > mHigh.x ){ mHigh.x = vertex.x; }
            if( vertex.y > mHigh.y ){ mHigh.y = vertex.y; }
            if( vertex.z > mHigh.z ){ mHigh.z = vertex.z; }
        }
        fanAssert( mLow.x <= mHigh.x && mLow.y <= mHigh.y && mLow.z <= mHigh.z );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<Vector3> AABB::GetCorners() const
    {
        return {
                mHigh, Vector3( mLow.x, mHigh.y, mHigh.z )// Top face
                , Vector3( mLow.x, mHigh.y, mLow.z ), Vector3( mHigh.x, mHigh.y, mLow.z ), Vector3( mHigh.x, mLow.y, mHigh.z )//Bot face
                , Vector3( mLow.x, mLow.y, mHigh.z ), mLow, Vector3( mHigh.x, mLow.y, mLow.z )
        };
    }

    //==================================================================================================================================================================================================
    // Fast Ray - Box Intersection
    // by Andrew Woo
    // from "Graphics Gems", Academic Press, 1990
    // Original code : https://github.com/erich666/GraphicsGems/blob/master/gems/RayBox.c
    //==================================================================================================================================================================================================
    bool AABB::RayCast( const Vector3& _origin, const Vector3& _direction, Vector3& _outIntersection ) const
    {
        const int RIGHT  = 0;
        const int LEFT   = 1;
        const int MIDDLE = 2;

        bool    inside = true;
        Vector3 quadrant( 0, 0, 0 );
        int     i;
        int     whichPlane;
        Vector3 maxT;
        Vector3 candidatePlane;

        // Find candidate planes; this loop can be avoided if
        // rays cast all from the eye(assume perpsective view)
        for( i = 0; i < 3; i++ )
        {
            if( _origin[i] < mLow[i] )
            {
                quadrant[i]       = LEFT;
                candidatePlane[i] = mLow[i];
                inside = false;
            }
            else if( _origin[i] > mHigh[i] )
            {
                quadrant[i]       = RIGHT;
                candidatePlane[i] = mHigh[i];
                inside = false;
            }
            else
            {
                quadrant[i] = MIDDLE;
            }
        }

        // Ray origin inside bounding box
        if( inside )
        {
            _outIntersection = _origin;
            return true;
        }


        // Calculate T distances to candidate planes
        for( i = 0; i < 3; i++ )
        {
            if( quadrant[i] != MIDDLE && _direction[i] != 0 )
            {
                maxT[i] = ( candidatePlane[i] - _origin[i] ) / _direction[i];
            }
            else
            {
                maxT[i] = -1;
            }
        }

        // Get largest of the maxT's for final choice of intersection
        whichPlane = 0;
        for( i     = 1; i < 3; i++ )
        {
            if( maxT[whichPlane] < maxT[i] )
            {
                whichPlane = i;
            }
        }

        // Check final candidate actually inside box
        if( maxT[whichPlane] < 0 )
        {
            return false;
        }

        for( i = 0; i < 3; i++ )
        {
            if( whichPlane != i )
            {
                _outIntersection[i] = _origin[i] + maxT[whichPlane] * _direction[i];
                if( _outIntersection[i] < mLow[i] || _outIntersection[i] > mHigh[i] )
                {
                    return false;
                }
            }
            else
            {
                _outIntersection[i] = candidatePlane[i];
            }
        }

        return true;
    }
}
