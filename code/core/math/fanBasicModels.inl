namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	inline std::vector< btVector3 > GetCube( const btVector3 _halfExtent )
	{
		const btVector3 v0( +_halfExtent[ 0 ], +_halfExtent[ 1 ], +_halfExtent[ 2 ] );
		const btVector3 v1( +_halfExtent[ 0 ], +_halfExtent[ 1 ], -_halfExtent[ 2 ] );
		const btVector3 v2( -_halfExtent[ 0 ], +_halfExtent[ 1 ], +_halfExtent[ 2 ] );
		const btVector3 v3( -_halfExtent[ 0 ], +_halfExtent[ 1 ], -_halfExtent[ 2 ] );
		const btVector3 v4( +_halfExtent[ 0 ], -_halfExtent[ 1 ], +_halfExtent[ 2 ] );
		const btVector3 v5( +_halfExtent[ 0 ], -_halfExtent[ 1 ], -_halfExtent[ 2 ] );
		const btVector3 v6( -_halfExtent[ 0 ], -_halfExtent[ 1 ], +_halfExtent[ 2 ] );
		const btVector3 v7( -_halfExtent[ 0 ], -_halfExtent[ 1 ], -_halfExtent[ 2 ] );

		return {
			 v0,v1,v2	,v1,v3,v2	// top
			,v6,v5,v4	,v7,v5,v6	// bot
			,v7,v6,v2	,v7,v2,v3
			,v6,v4,v0	,v6,v0,v2
			,v4,v5,v0	,v5,v1,v0
			,v7,v1,v5	,v7,v3,v1
		};
	}

	//================================================================================================================================
	//================================================================================================================================
	inline std::vector<btVector3> GetSphere( const float _radius, const int _numSubdivisions )
	{
		const float goldenRatio = ( 1 + std::sqrtf( 5 ) ) / 2.f;
		const float a = _radius * 2.f / sqrtf( goldenRatio * sqrtf( 5.f ) );
		const float x = a / 2;
		const float y = goldenRatio * a / 2;

		const btVector3 v0( x, y, 0.f );
		const btVector3 v1( 0.f, x, y );
		const btVector3 v2( y, 0.f, x );
		const btVector3 v3( -x, y, 0.f );
		const btVector3 v4( 0.f, -x, y );
		const btVector3 v5( -y, 0.f, x );
		const btVector3 v6( x, -y, 0.f );
		const btVector3 v7( 0.f, x, -y );
		const btVector3 v8( y, 0.f, -x );
		const btVector3 v9( -x, -y, 0.f );
		const btVector3 v10( 0.f, -x, -y );
		const btVector3 v11( -y, 0.f, -x );

		std::vector< btVector3 > icosahedron0{
			v0,v1, v2	 		,v2,v1, v4
			,v2,v4, v6			,v6,v4, v9
			,v1,v0, v3	 		,v1,v5, v4
			,v4,v5, v9 			,v1,v3, v5
			,v5,v11,v9 			,v5,v3,v11
			,v6,v8,v2 			,v6,v10,v8
			,v8,v10,v7 			,v6,v9,v10
			,v9,v11,v10 		,v7,v0,v8
			,v10,v11,v7 		,v0,v7,v3
			,v7,v11,v3			,v0,v2,v8
		};

		std::vector< btVector3 > icosahedron1;
		std::vector< btVector3 >& currentIcosahedron = icosahedron0;
		std::vector< btVector3 >& nextIcosahedron = icosahedron1;

		for ( int subdivisionIndex = 0; subdivisionIndex < _numSubdivisions; subdivisionIndex++ )
		{
			nextIcosahedron.clear();
			for ( int triangleIndex = 0; triangleIndex < (int)currentIcosahedron.size() / 3; triangleIndex++ )
			{
				const btVector3 vertex0 = currentIcosahedron[ 3 * triangleIndex + 0 ];
				const btVector3 vertex1 = currentIcosahedron[ 3 * triangleIndex + 1 ];
				const btVector3 vertex2 = currentIcosahedron[ 3 * triangleIndex + 2 ];
				const btVector3 vertex01 = _radius * ( ( vertex0 + vertex1 ) / 2 ).normalized();
				const btVector3 vertex12 = _radius * ( ( vertex1 + vertex2 ) / 2 ).normalized();
				const btVector3 vertex20 = _radius * ( ( vertex2 + vertex0 ) / 2 ).normalized();

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
			std::vector< btVector3 >& tmp = currentIcosahedron;
			currentIcosahedron = nextIcosahedron;
			nextIcosahedron = tmp;
		}

		return currentIcosahedron;
	}

	//================================================================================================================================
	//================================================================================================================================
	inline std::vector<btVector3> GetCone( const float _radius, const float _height, const int _numSubdivisions )
	{
		btVector3 center( 0, 0, 0 );
		btVector3 top( 0, _height, 0 );

		std::vector<btVector3> baseVertices;
		baseVertices.reserve( _numSubdivisions );
		for ( int subdivisionIndex = 0; subdivisionIndex < _numSubdivisions; subdivisionIndex++ )
		{
			float angle = 2.f * subdivisionIndex * SIMD_PI / _numSubdivisions;
			btVector3 point( std::cosf( angle ), 0, std::sinf( angle ) );
			baseVertices.push_back( _radius * point );
		}

		std::vector<btVector3> vertices;
		for ( int subdivisionIndex = 0; subdivisionIndex < _numSubdivisions; subdivisionIndex++ )
		{

			vertices.push_back( baseVertices[ subdivisionIndex ] );
			vertices.push_back( baseVertices[ ( subdivisionIndex + 1 ) % _numSubdivisions ] );
			vertices.push_back( center );

			vertices.push_back( baseVertices[ subdivisionIndex ] );
			vertices.push_back( top );
			vertices.push_back( baseVertices[ ( subdivisionIndex + 1 ) % _numSubdivisions ] );
		}
		return vertices;
	}
}


