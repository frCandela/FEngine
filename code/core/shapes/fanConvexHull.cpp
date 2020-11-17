#include "core/shapes/fanConvexHull.hpp"

#include "quickhull/QuickHull.hpp"
#include "bullet/LinearMath/btConvexHull.h"
#include "core/fanDebug.hpp"
#include "core/shapes/fanTriangle.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ConvexHull::ComputeBulletHull( const std::vector<btVector3>& _pointCloud )
	{
		Clear();

		// Calculates hull
		HullResult	hull;
		HullDesc	desc( QF_TRIANGLES, ( int ) _pointCloud.size(), _pointCloud.data() );
		HullLibrary lib;
		if ( lib.CreateConvexHull( desc, hull ) == QE_FAIL )
		{
			Debug::Warning( "unable to compute convex hull" );
		}
		else
		{
			// Copy indices
			mIndices.reserve( hull.mNumIndices );
			for ( unsigned indexIndex = 0; indexIndex < hull.mNumIndices; indexIndex++ )
			{
				mIndices.push_back( ( uint32_t ) hull.m_Indices[ indexIndex ] );
			}

			// Copy vertices
			mVertices.reserve( hull.mNumOutputVertices );
			for ( unsigned vertexIndex = 0; vertexIndex < hull.mNumOutputVertices; vertexIndex++ )
			{
				mVertices.push_back( hull.m_OutputVertices[ vertexIndex ] );
			}
		}

		lib.ReleaseResult( hull );
	}

	//========================================================================================================
	//========================================================================================================
	void ConvexHull::ComputeQuickHull( const std::vector<btVector3>& _pointCloud )
	{
		Clear();

		// Convert data to quickhull vectices
		std::vector<quickhull::Vector3<float>> pointCloud;
		pointCloud.reserve( _pointCloud.size() );
		for ( int point = 0; point < (int)_pointCloud.size(); point++ )
		{
			const btVector3& vertex = _pointCloud[ point ];
			pointCloud.push_back( { vertex[ 0 ],vertex[ 1 ],vertex[ 2 ] } );
		}

		// Calculates hull
		quickhull::QuickHull<float> qh;
		quickhull::ConvexHull<float> hull;
		hull = qh.getConvexHull( pointCloud, true, false );

		// Copy indices
		const std::vector<size_t>& indexBuffer = hull.getIndexBuffer();
		mIndices.reserve( indexBuffer.size() );
        fanAssert( indexBuffer.size() % 3 == 0 );
		for ( int indexIndex = 0; indexIndex < (int)indexBuffer.size() / 3; indexIndex++ )
		{
			mIndices.push_back( ( uint32_t ) indexBuffer[3 * indexIndex + 0 ] );
			mIndices.push_back( ( uint32_t ) indexBuffer[3 * indexIndex + 2 ] );
			mIndices.push_back( ( uint32_t ) indexBuffer[3 * indexIndex + 1 ] );
		}

		// Copy vertices
		const quickhull::VertexDataSource<float>& vertexBuffer = hull.getVertexBuffer();
		mVertices.reserve( vertexBuffer.size() );
		for ( int vertexIndex = 0; vertexIndex < (int)vertexBuffer.size(); vertexIndex++ )
		{
			quickhull::Vector3<float> vertex = vertexBuffer[ vertexIndex ];
			mVertices.push_back( btVector3( vertex.x, vertex.y, vertex.z ) );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ConvexHull::Clear()
	{
		mVertices.clear();
		mIndices.clear();
	}

	//========================================================================================================
	// Raycast on all triangles of the convex hull
	//========================================================================================================
    bool ConvexHull::RayCast( const btVector3 _origin,
                              const btVector3 _direction,
                              btVector3& _outIntersection ) const
    {
		btVector3 intersection;
		float closestDistance = std::numeric_limits<float>::max();
		for ( int triIndex = 0; triIndex < (int)mIndices.size() / 3; triIndex++ )
		{
			const btVector3 v0 = mVertices[ mIndices[3 * triIndex + 0 ] ];
			const btVector3 v1 = mVertices[ mIndices[3 * triIndex + 1 ] ];
			const btVector3 v2 = mVertices[ mIndices[3 * triIndex + 2 ] ];
			const Triangle triangle( v0, v1, v2 );

			if ( triangle.RayCast( _origin, _direction, intersection ) )
			{
				float distance = intersection.distance( _origin );
				if ( distance < closestDistance )
				{
					closestDistance = distance;
					_outIntersection = intersection;
				}
			}
		}
		return closestDistance != std::numeric_limits<float>::max();
	}
}

