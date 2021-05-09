#include "core/shapes/fanConvexHull.hpp"

#include "quickhull/QuickHull.hpp"
#include "core/fanDebug.hpp"
#include "core/shapes/fanTriangle.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConvexHull::ComputeQuickHull( const std::vector<Vector3>& _pointCloud )
    {
        Clear();

        // Convert data to quickhull vectices
        std::vector<quickhull::Vector3<float>> pointCloud;
        pointCloud.reserve( _pointCloud.size() );
        for( int point = 0; point < (int)_pointCloud.size(); point++ )
        {
            const Vector3& vertex = _pointCloud[point];
            pointCloud.push_back( { vertex.x.ToFloat(), vertex.y.ToFloat(), vertex.z.ToFloat() } );
        }

        // Calculates hull
        quickhull::QuickHull<float>  qh;
        quickhull::ConvexHull<float> hull;
        hull = qh.getConvexHull( pointCloud, true, false );

        // Copy indices
        const std::vector<size_t>& indexBuffer = hull.getIndexBuffer();
        mIndices.reserve( indexBuffer.size() );
        fanAssert( indexBuffer.size() % 3 == 0 );
        for( int indexIndex = 0; indexIndex < (int)indexBuffer.size() / 3; indexIndex++ )
        {
            mIndices.push_back( (uint32_t)indexBuffer[3 * indexIndex + 0] );
            mIndices.push_back( (uint32_t)indexBuffer[3 * indexIndex + 2] );
            mIndices.push_back( (uint32_t)indexBuffer[3 * indexIndex + 1] );
        }

        // Copy vertices
        const quickhull::VertexDataSource<float>& vertexBuffer = hull.getVertexBuffer();
        mVertices.reserve( vertexBuffer.size() );
        for( int vertexIndex = 0; vertexIndex < (int)vertexBuffer.size(); vertexIndex++ )
        {
            quickhull::Vector3<float> vertex = vertexBuffer[vertexIndex];
            mVertices.push_back( Vector3( Fixed::FromFloat( vertex.x ), Fixed::FromFloat( vertex.y ), Fixed::FromFloat( vertex.z ) ) );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConvexHull::Clear()
    {
        mVertices.clear();
        mIndices.clear();
    }

    //==================================================================================================================================================================================================
    // Raycast on all triangles of the convex hull
    //==================================================================================================================================================================================================
    bool ConvexHull::RayCast( const Vector3& _origin, const Vector3& _direction, Vector3& _outIntersection ) const
    {
        Vector3  intersection;
        Fixed    closestDistance = Fixed::sMaxValue;
        for( int triIndex        = 0; triIndex < (int)mIndices.size() / 3; triIndex++ )
        {
            const Vector3  v0 = mVertices[mIndices[3 * triIndex + 0]];
            const Vector3  v1 = mVertices[mIndices[3 * triIndex + 1]];
            const Vector3  v2 = mVertices[mIndices[3 * triIndex + 2]];
            const Triangle triangle( v0, v1, v2 );

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
}

