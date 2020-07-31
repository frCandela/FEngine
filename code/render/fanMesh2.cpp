#include "render/fanMesh2.hpp"

#include "render/fanGLTFImporter2.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/shapes/fanConvexHull.hpp"
#include "core/shapes/fanTriangle.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	bool Mesh2::LoadFromFile( const std::string& _path )
	{
		GLTFImporter2 importer;
		if ( importer.Load( _path ) )
		{
			if ( !importer.GetMesh( *this ) )
			{
				Debug::Get() << "Failed to load mesh : " << mPath << Debug::Endl();
				return false;
			}

			OptimizeVertices();
			GenerateConvexHull();

            mPath = _path;
			return true;
		}

		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh2::LoadFromVertices( const std::vector<Vertex>& _vertices )
	{
		mVertices = _vertices;

		// Generate fake indices
        mIndices.clear();
        mIndices.reserve( _vertices.size() );
		for ( int vertIndex = 0; vertIndex < (int)_vertices.size(); vertIndex++ )
		{
            mIndices.push_back( vertIndex );
		}

		// Cleanup
		OptimizeVertices();
		GenerateConvexHull();
		return true;
	}


	//================================================================================================================================
	// Removes duplicates vertices & generates a corresponding index buffer
	//================================================================================================================================
	void Mesh2::OptimizeVertices()
	{

		if ( !mOptimizeVertices ) { return; }

		std::unordered_map<Vertex, uint32_t> verticesMap = {};

		std::vector<Vertex> uniqueVertices;
		std::vector<uint32_t>	uniqueIndices;

		for ( int indexIndex = 0; indexIndex < (int)mIndices.size(); indexIndex++ )
		{
			Vertex vertex = mVertices[ mIndices[ indexIndex ] ];
			auto it = verticesMap.find( vertex );
			if ( it == verticesMap.end() )
			{
				verticesMap[ vertex ] = static_cast< uint32_t >( uniqueVertices.size() );
				uniqueIndices.push_back( static_cast< uint32_t >( uniqueVertices.size() ) );
				uniqueVertices.push_back( vertex );
			}
			else
			{
				uniqueIndices.push_back( it->second );
			}
		}
        mVertices = uniqueVertices;
        mIndices = uniqueIndices;
	}

	//================================================================================================================================
	// Creates a convex hull from the mesh geometry
	//================================================================================================================================
	void  Mesh2::GenerateConvexHull()
	{
		if ( mVertices.empty() || !mAutoUpdateHull ) { return; }

		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve( mVertices.size() );
		for ( int point = 0; point < (int)mVertices.size(); point++ )
		{
			Vertex& vertex = mVertices[ point ];
			pointCloud.push_back( btVector3( vertex.pos.x, vertex.pos.y, vertex.pos.z ) );
		}
		mConvexHull.ComputeQuickHull( pointCloud );
	}

	//================================================================================================================================
	// Raycast on all triangles of the convex hull
	//================================================================================================================================
	bool Mesh2::RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const
	{
		btVector3 intersection;
		float closestDistance = std::numeric_limits<float>::max();
		for ( int triIndex = 0; triIndex < (int)mIndices.size() / 3; triIndex++ )
		{
			const btVector3 v0 = ToBullet( mVertices[ mIndices[ 3 * triIndex + 0 ] ].pos );
			const btVector3 v1 = ToBullet( mVertices[ mIndices[ 3 * triIndex + 1 ] ].pos );
			const btVector3 v2 = ToBullet( mVertices[ mIndices[ 3 * triIndex + 2 ] ].pos );
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