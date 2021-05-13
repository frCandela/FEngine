#include <cmath>

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    inline std::vector<Vector3> GetCube( const Vector3 _halfExtent )
    {
        const Vector3 v0( +_halfExtent[0], +_halfExtent[1], +_halfExtent[2] );
        const Vector3 v1( +_halfExtent[0], +_halfExtent[1], -_halfExtent[2] );
        const Vector3 v2( -_halfExtent[0], +_halfExtent[1], +_halfExtent[2] );
        const Vector3 v3( -_halfExtent[0], +_halfExtent[1], -_halfExtent[2] );
        const Vector3 v4( +_halfExtent[0], -_halfExtent[1], +_halfExtent[2] );
        const Vector3 v5( +_halfExtent[0], -_halfExtent[1], -_halfExtent[2] );
        const Vector3 v6( -_halfExtent[0], -_halfExtent[1], +_halfExtent[2] );
        const Vector3 v7( -_halfExtent[0], -_halfExtent[1], -_halfExtent[2] );

        return { v0, v1, v2, v3, v4, v5, v6, v7 };
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    inline std::vector<Vector3> GetSphere( const Fixed _radius, const int _numSubdivisions )
    {
        const Fixed goldenRatio = ( 1 + Fixed::Sqrt( 5 ) ) / 2;
        const Fixed a           = _radius * 2 / Fixed::Sqrt( goldenRatio * Fixed::Sqrt( 5 ) );
        const Fixed x           = a / 2;
        const Fixed y           = goldenRatio * a / 2;

        const Vector3 v0( x, y, 0 );
        const Vector3 v1( 0, x, y );
        const Vector3 v2( y, 0, x );
        const Vector3 v3( -x, y, 0 );
        const Vector3 v4( 0, -x, y );
        const Vector3 v5( -y, 0, x );
        const Vector3 v6( x, -y, 0 );
        const Vector3 v7( 0, x, -y );
        const Vector3 v8( y, 0, -x );
        const Vector3 v9( -x, -y, 0 );
        const Vector3 v10( 0, -x, -y );
        const Vector3 v11( -y, 0, -x );

        std::vector<Vector3> icosahedron0 {
                v0,
                v1,
                v2,
                v2,
                v1,
                v4,
                v2,
                v4,
                v6,
                v6,
                v4,
                v9,
                v1,
                v0,
                v3,
                v1,
                v5,
                v4,
                v4,
                v5,
                v9,
                v1,
                v3,
                v5,
                v5,
                v11,
                v9,
                v5,
                v3,
                v11,
                v6,
                v8,
                v2,
                v6,
                v10,
                v8,
                v8,
                v10,
                v7,
                v6,
                v9,
                v10,
                v9,
                v11,
                v10,
                v7,
                v0,
                v8,
                v10,
                v11,
                v7,
                v0,
                v7,
                v3,
                v7,
                v11,
                v3,
                v0,
                v2,
                v8
        };

        std::vector<Vector3> icosahedron1;
        std::vector<Vector3>& currentIcosahedron = icosahedron0;
        std::vector<Vector3>& nextIcosahedron    = icosahedron1;

        for( int subdivisionIndex = 0; subdivisionIndex < _numSubdivisions; subdivisionIndex++ )
        {
            nextIcosahedron.clear();
            for( int triangleIndex = 0; triangleIndex < (int)currentIcosahedron.size() / 3; triangleIndex++ )
            {
                const Vector3 vertex0  = currentIcosahedron[3 * triangleIndex + 0];
                const Vector3 vertex1  = currentIcosahedron[3 * triangleIndex + 1];
                const Vector3 vertex2  = currentIcosahedron[3 * triangleIndex + 2];
                const Vector3 vertex01 = _radius * ( ( vertex0 + vertex1 ) / 2 ).Normalized();
                const Vector3 vertex12 = _radius * ( ( vertex1 + vertex2 ) / 2 ).Normalized();
                const Vector3 vertex20 = _radius * ( ( vertex2 + vertex0 ) / 2 ).Normalized();

                nextIcosahedron.push_back( vertex01 );
                nextIcosahedron.push_back( vertex12 );
                nextIcosahedron.push_back( vertex20 );

                nextIcosahedron.push_back( vertex0 );
                nextIcosahedron.push_back( vertex01 );
                nextIcosahedron.push_back( vertex20 );

                nextIcosahedron.push_back( vertex1 );
                nextIcosahedron.push_back( vertex12 );
                nextIcosahedron.push_back( vertex01 );

                nextIcosahedron.push_back( vertex2 );
                nextIcosahedron.push_back( vertex20 );
                nextIcosahedron.push_back( vertex12 );
            }
            std::vector<Vector3>& tmp = currentIcosahedron;
            currentIcosahedron = nextIcosahedron;
            nextIcosahedron    = tmp;
        }

        return currentIcosahedron;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    inline std::vector<Vector3> GetCone( const Fixed _radius, const Fixed _height, const int _numSubdivisions )
    {
        Vector3 center( 0, 0, 0 );
        Vector3 top( 0, _height, 0 );

        std::vector<Vector3> baseVertices;
        baseVertices.reserve( _numSubdivisions );
        for( int subdivisionIndex             = 0; subdivisionIndex < _numSubdivisions; subdivisionIndex++ )
        {
            Fixed   angle = 2 * subdivisionIndex * Fixed::sPi / _numSubdivisions;
            Vector3 point( Fixed::Cos( angle ), 0, Fixed::Sin( angle ) );
            baseVertices.push_back( _radius * point );
        }

        std::vector<Vector3> vertices;
        for( int             subdivisionIndex = 0; subdivisionIndex < _numSubdivisions; subdivisionIndex++ )
        {

            vertices.push_back( baseVertices[subdivisionIndex] );
            vertices.push_back( baseVertices[( subdivisionIndex + 1 ) % _numSubdivisions] );
            vertices.push_back( center );

            vertices.push_back( baseVertices[subdivisionIndex] );
            vertices.push_back( top );
            vertices.push_back( baseVertices[( subdivisionIndex + 1 ) % _numSubdivisions] );
        }
        return vertices;
    }
}


