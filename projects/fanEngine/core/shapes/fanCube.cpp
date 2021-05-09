#include <limits>
#include "core/shapes/fanCube.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    Cube::Cube( const Vector3 _position, Fixed const _halfSize ) : mPosition( _position ), mHalfSize( _halfSize )
    {
        InitCube();
    }

    //========================================================================================================
    //========================================================================================================
    void Cube::SetPosition( const Vector3 _pos )
    {
        if( mPosition != _pos )
        {
            mPosition = _pos;
            InitCube();
        }
    }

    //========================================================================================================
    //========================================================================================================
    bool Cube::RayCast( const Vector3 _origin, const Vector3 _direction, Vector3& _outIntersection ) const
    {
        Vector3 intersection;
        Fixed   closestDistance = Fixed::sMaxValue;
        for( const Triangle& triangle : mTriangles )
        {
            if( triangle.RayCast( _origin, _direction, intersection ) )
            {
                Fixed distance = Vector3::Distance( intersection, _origin );
                if( distance < closestDistance )
                {
                    closestDistance  = distance;
                    _outIntersection = intersection;
                }
            }
        }
        return closestDistance != Fixed::sMaxValue;
    }

    //========================================================================================================
    //========================================================================================================
    void Cube::InitCube()
    {
        const Vector3 e1 = mPosition + Vector3( -mHalfSize, -mHalfSize, -mHalfSize );
        const Vector3 e2 = mPosition + Vector3( -mHalfSize, -mHalfSize, mHalfSize );
        const Vector3 e3 = mPosition + Vector3( -mHalfSize, mHalfSize, -mHalfSize );
        const Vector3 e4 = mPosition + Vector3( -mHalfSize, mHalfSize, mHalfSize );
        const Vector3 e5 = mPosition + Vector3( mHalfSize, -mHalfSize, -mHalfSize );
        const Vector3 e6 = mPosition + Vector3( mHalfSize, -mHalfSize, mHalfSize );
        const Vector3 e7 = mPosition + Vector3( mHalfSize, mHalfSize, -mHalfSize );
        const Vector3 e8 = mPosition + Vector3( mHalfSize, mHalfSize, mHalfSize );

        mTriangles[0]  = { e1, e2, e4 };
        mTriangles[1]  = { e1, e3, e4 };
        mTriangles[2]  = { e5, e6, e8 };
        mTriangles[3]  = { e5, e7, e8 };
        mTriangles[4]  = { e2, e6, e8 };
        mTriangles[5]  = { e2, e4, e8 };
        mTriangles[6]  = { e1, e5, e7 };
        mTriangles[7]  = { e1, e3, e7 };
        mTriangles[8]  = { e3, e4, e8 };
        mTriangles[9]  = { e3, e7, e8 };
        mTriangles[10] = { e1, e2, e6 };
        mTriangles[11] = { e1, e5, e6 };
    }
}

