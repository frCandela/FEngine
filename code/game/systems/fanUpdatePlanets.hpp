#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanTransform.hpp"
#include "game/components/fanPlanet.hpp"

namespace fan
{
	//========================================================================================================
	// moves the planets in circles around their origin ( zero for now )
	//========================================================================================================
	struct SMovePlanets : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<SceneNode>() |
				_world.GetSignature<Transform>() |
				_world.GetSignature<Planet>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			Time& time = _world.GetSingleton<Time>();
			const float currentTime = time.mFrameIndex * time.mLogicDelta;

			auto transformIt = _view.begin<Transform>();
			auto planetIt = _view.begin<Planet>();
			auto sceneNodeIt = _view.begin<SceneNode>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++planetIt, ++sceneNodeIt )
			{
				SceneNode& sceneNode = *sceneNodeIt;
				Transform& transform = *transformIt;
				const Planet& planet = *planetIt;

				float const planetTime = -planet.mSpeed * currentTime;
                btVector3 position( std::cosf( planetTime + planet.mPhase ),
                                    0,
                                    std::sinf( planetTime + planet.mPhase ) );
				transform.SetPosition( /*parentTransform.getOrigin()*/ planet.mRadius * position );

				sceneNode.AddFlag( SceneNode::BoundsOutdated );
			}
		}
	};

	//========================================================================================================
	// generates a light mesh from the planets positions & radius
	//========================================================================================================
	struct SGenerateLightMesh : EcsSystem
	{
		//================================================================
		// helper struct for the S_GenerateLightMesh system
		//================================================================
		struct OrientedSegment
		{
			enum OpenSide { RIGHT = 1, LEFT = 2, BOTH = RIGHT | LEFT };

			btVector3 direction;
			OpenSide  openSide;
			float norm;
		};

		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform>() |
				_world.GetSignature<Planet>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			SCOPED_PROFILE( ecs_solar_erup )

				if( _delta == 0.f ) { return; }

			SunLight& sunLight = _world.GetSingleton<SunLight>();

			// Generates occlusion rays for each planet
			std::vector< OrientedSegment > segments;
			segments.reserve( 2 * _view.Size() );

			auto transformIt = _view.begin<Transform>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt )
			{
				Transform& transform = *transformIt;

				btVector3& scale = transform.mScale;

				const btVector3 planetPos = transform.GetPosition();
				const btVector3 direction = planetPos - btVector3::Zero();
				const btVector3 left = btVector3::Up().cross( direction ).normalized();

				const btVector3 leftDirection = 0.5f * scale.getX() * left;
				const btVector3 planetLeft = planetPos + leftDirection;
				const btVector3 planetRight = planetPos - leftDirection;
				const float norm = planetRight.norm();

				segments.push_back( { planetRight, OrientedSegment::RIGHT, norm } );
				segments.push_back( { planetLeft, OrientedSegment::LEFT, norm } );
			}

			// Sort the segments in ascending order ( counter clockwise )
			std::sort( std::begin( segments ), std::end( segments ),
				[]( OrientedSegment& _s1, OrientedSegment& _s2 )
			{
                return SignedAngle( btVector3::Left(),
                                    _s1.direction,
                                    btVector3::Up() ) < SignedAngle( btVector3::Left(),
                                                                     _s2.direction,
                                                                     btVector3::Up() );
			} );

			// Finds the starting point of mesh generation loop
			int startIndex = 0;
			{
				// Finds the index of a RIGHT axis that has a minimal number of nested levels of planets
				int startIndexDepth = 10000;
				int depth = 0;	//			
				for( int axisIndex = 0; axisIndex < (int)segments.size(); axisIndex++ )
				{
					OrientedSegment& axis = segments[axisIndex];
					depth += axis.openSide == OrientedSegment::RIGHT ? 1 : -1;
					if( axis.openSide == OrientedSegment::RIGHT && depth < startIndexDepth )
					{
						startIndex = axisIndex;
						startIndexDepth = depth;
					}
				}
			}

			// generates the mesh
			std::vector<Vertex>	vertices;
			vertices.reserve( 3 * _view.Size() );
			const float minGapRadians = btRadians( sunLight.mSubAngle );
			std::set<float> norms;	// Stores the nested opening segments norms
			for( int axisIndex = 0; axisIndex < (int)segments.size(); axisIndex++ )
			{
				const int index = ( axisIndex + startIndex ) % segments.size();
				const int indexNext = ( index + 1 ) % segments.size();
				OrientedSegment& axis = segments[index];
				OrientedSegment& axisNext = segments[indexNext];

				float length = 0.f;
				if( axis.openSide == OrientedSegment::RIGHT )
				{
					// Easy case of an opening segment
					norms.insert( axis.norm );
					length = *norms.begin(); // Gets the smallest norm
				}
				else if( axis.openSide == OrientedSegment::LEFT )
				{
					norms.erase( axis.norm );
					if( !norms.empty() )
					{
						length = *norms.begin(); // Easy case of a closing segment
					}
					else
					{
						if( axisNext.openSide == OrientedSegment::RIGHT )
						{
							// Empty space with no planets -> fills the space with triangles
							float angle = SignedAngle( axis.direction, axisNext.direction, btVector3::Up() );
							if( angle > minGapRadians ) // gap is too large -> subdivise it
							{

								const int numSubdivistions = int( angle / minGapRadians ) + 1;
								const float subdivisionAngle = angle / numSubdivistions;
								btTransform rotate( btQuaternion( btVector3::Up(), subdivisionAngle ) );
								btVector3 subAxisNext = sunLight.mRadius * axis.direction / axis.norm;
								for( int subAxisIndex = 0; subAxisIndex < numSubdivistions; subAxisIndex++ )
								{
									btVector3 subAxis = subAxisNext;
									subAxisNext = rotate * subAxisNext;
									sunLight.AddSunTriangle( vertices, subAxis, subAxisNext );
								}
							}
							else // gap size is small enough
							{
								length = sunLight.mRadius;
							}
						}
						else
						{
							//Debug::Warning( "degenerate case ???" );
						}
					}
				}

				// Generate a light triangle based on the processed segment length
				if( length > 0.f )
				{
					const btVector3 v0 = length * axis.direction / axis.norm;
					const btVector3 v1 = length * axisNext.direction / axisNext.norm;
					sunLight.AddSunTriangle( vertices, v0, v1 );
				}
			}

			// Load mesh
			sunLight.mMesh->LoadFromVertices( vertices );
		}
	};
}