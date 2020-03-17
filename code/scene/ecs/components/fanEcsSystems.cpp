//#include "scene/ecs/fanECSSystems.hpp"

namespace fan
{
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void ecsParticleSunlightOcclusionSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 												  , ComponentData< ecsPosition >& _positions
// 												  , ComponentData< ecsParticle >&						/*_particles*/
// 												  , ComponentData< ecsSunlightParticleOcclusion >&	/*_occlusion*/ )
// 	{
// 		// Get singleton components 
// 		ecsSunLightMesh_s& sunLight = _singletonComponents.GetComponent<ecsSunLightMesh_s>();
// 
// 		for ( int entity = 0; entity < _count; entity++ )
// 		{
// 			ecsComponentsKey& key = _entitiesData[ entity ];
// 
// 			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 			{
// 				// get data
// 				btVector3& position = _positions.At( key ).position;
// 
// 				// raycast on the light mesh
// 				const btVector3 rayOrigin = btVector3( position[ 0 ], 1.f, position[ 2 ] );
// 				btVector3 outIntersection;
// 				bool isInsideSunlight = sunLight.mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );
// 				if ( !isInsideSunlight )
// 				{
// 					key.Kill();
// 				}
// 			}
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void ecsPlanetsSystem::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& /*_singletonComponents*/
// 								, ComponentData< ecsGameobject >& _gameobjects
// 								, ComponentData< ecsTranform >& _transforms
// 								, ComponentData< ecsPlanet >& _planets
// 								, ComponentData< ecsFlags >& _flags )
// 	{
// 		for ( int entity = 0; entity < _count; entity++ )
// 		{
// 			ecsComponentsKey& key = _entitiesData[ entity ];
// 			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 			{
// 				btTransform& transform = _transforms.At( key ).transform;
// 				ecsPlanet& planet = _planets.At( key );
// 				const btTransform& parentTransform = _gameobjects.At( key ).gameobject->GetParent()->GetTransform().GetBtTransform();
// 				ecsFlags& flags = _flags.At( key );
// 
// 				planet.time += _delta;
// 				float const time = -planet.speed * planet.time;
// 				btVector3 position( std::cosf( time + planet.phase ), 0, std::sinf( time + planet.phase ) );
// 
// 				if ( std::abs( time ) > SIMD_2_PI ) { planet.time -= SIMD_2_PI / std::abs( planet.speed ); }
// 
// 				transform.setOrigin( parentTransform.getOrigin() + planet.radius * position );
// 				flags.flags |= ecsFlags::OUTDATED_AABB;
// 			}
// 		}
//	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void ecsSolarEruptionMeshSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 										  , ComponentData< ecsPlanet >&	/*_planets*/
// 										  , ComponentData< ecsTranform >& _transforms
// 										  , ComponentData< ecsScaling >& _scaling )
// 
// 	{
// 		SCOPED_PROFILE( ecs_solar_erup )
// 
// 			// Get singleton components 
// 			ecsSunLightMesh_s& sunLight = _singletonComponents.GetComponent<ecsSunLightMesh_s>();
// 
// 		// Get the interesting matching keys
// 		std::vector<ecsComponentsKey*> interestingKeys;
// 		interestingKeys.reserve( 64 );
// 		for ( int entity = 0; entity < _count; entity++ )
// 		{
// 			ecsComponentsKey& key = _entitiesData[ entity ];
// 			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 			{
// 				interestingKeys.push_back( &key );
// 			}
// 		}
// 
// 		// Generates occlusion rays for each planet
// 		std::vector< OrientedSegment > segments;
// 		segments.reserve( 2 * interestingKeys.size() );
// 		for ( int keyIndex = 0; keyIndex < interestingKeys.size(); keyIndex++ )
// 		{
// 			ecsComponentsKey& key = *interestingKeys[ keyIndex ];
// 			btTransform& transform = _transforms.At( key ).transform;
// 			btVector3& scale = _scaling.At( key ).scale;
// 
// 			const btVector3 planetPos = transform.getOrigin();
// 			const btVector3 direction = planetPos - btVector3::Zero();
// 			const btVector3 left = btVector3::Up().cross( direction ).normalized();
// 
// 			const btVector3 leftDirection = scale.getX() * left;
// 			const btVector3 planetLeft = planetPos + leftDirection;
// 			const btVector3 planetRight = planetPos - leftDirection;
// 			const float norm = planetRight.norm();
// 
// 			segments.push_back( { planetRight, OrientedSegment::RIGHT, norm } );
// 			segments.push_back( { planetLeft, OrientedSegment::LEFT, norm } );
// 		}
// 
// 		// Sort the segments in ascending order ( counter clockwise )
// 		std::sort( std::begin( segments ), std::end( segments ),
// 				   [] ( OrientedSegment& _s1, OrientedSegment& _s2 )
// 		{
// 			return btVector3::Left().SignedAngle( _s1.direction, btVector3::Up() ) < btVector3::Left().SignedAngle( _s2.direction, btVector3::Up() );
// 		} );
// 
// 		// Finds the starting point of mesh generation loop
// 		int startIndex = 0;
// 		{
// 			// Finds the index of a RIGHT axis that has a minimal number of nested levels of planets	(depth)	
// 			int startIndexDepth = 10000;
// 			int depth = 0;	//			
// 			for ( int axisIndex = 0; axisIndex < segments.size(); axisIndex++ )
// 			{
// 				OrientedSegment& axis = segments[ axisIndex ];
// 				depth += axis.openSide == OrientedSegment::RIGHT ? 1 : -1;
// 				if ( axis.openSide == OrientedSegment::RIGHT && depth < startIndexDepth )
// 				{
// 					startIndex = axisIndex;
// 					startIndexDepth = depth;
// 				}
// 			}
// 		}
// 
// 		// generates the mesh
// 		std::vector<Vertex>	vertices;
// 		vertices.reserve( 3 * interestingKeys.size() );
// 		const float minGapRadians = btRadians( sunLight.subAngle );
// 		std::set<float> norms;	// Stores the nested opening segments norms
// 		for ( int axisIndex = 0; axisIndex < segments.size(); axisIndex++ )
// 		{
// 			const int index = ( axisIndex + startIndex ) % segments.size();
// 			const int indexNext = ( index + 1 ) % segments.size();
// 			OrientedSegment& axis = segments[ index ];
// 			OrientedSegment& axisNext = segments[ indexNext ];
// 
// 			float length = 0.f;
// 			if ( axis.openSide == OrientedSegment::RIGHT )
// 			{
// 				// Easy case of an opening segment
// 				norms.insert( axis.norm );
// 				length = *norms.begin(); // Gets the smallest norm
// 			}
// 			else if ( axis.openSide == OrientedSegment::LEFT )
// 			{
// 				norms.erase( axis.norm );
// 				if ( !norms.empty() )
// 				{
// 					length = *norms.begin(); // Easy case of a closing segment
// 				}
// 				else
// 				{
// 					if ( axisNext.openSide == OrientedSegment::RIGHT )
// 					{
// 						// Empty space with no planets -> fills the space with triangles
// 						float angle = axis.direction.SignedAngle( axisNext.direction, btVector3::Up() );
// 						if ( angle > minGapRadians ) // gap is too large -> subdivise it
// 						{
// 
// 							const int numSubdivistions = int( angle / minGapRadians ) + 1;
// 							const float subdivisionAngle = angle / numSubdivistions;
// 							btTransform rotate( btQuaternion( btVector3::Up(), subdivisionAngle ) );
// 							btVector3 subAxisNext = sunLight.radius * axis.direction / axis.norm;
// 							for ( int subAxisIndex = 0; subAxisIndex < numSubdivistions; subAxisIndex++ )
// 							{
// 								btVector3 subAxis = subAxisNext;
// 								subAxisNext = rotate * subAxisNext;
// 								sunLight.AddSunTriangle( vertices, subAxis, subAxisNext );
// 							}
// 						}
// 						else // gap size is small enough
// 						{
// 							length = sunLight.radius;
// 						}
// 					}
// 					else
// 					{
// 						// I think this is a degenerate case that doesn't need to be considered #provemewrong
// 						////Debug::Warning( "VIDE JURIDIQUE" );
// 					}
// 				}
// 			}
// 
// 			// Generate a light triangle based on the processed segment length
// 			if ( length > 0.f )
// 			{
// 				const btVector3 v0 = length * axis.direction / axis.norm;
// 				const btVector3 v1 = length * axisNext.direction / axisNext.norm;
// 				sunLight.AddSunTriangle( vertices, v0, v1 );
// 			}
// 		}
// 
// 		// Load mesh
// 		sunLight.mesh->LoadFromVertices( vertices );
// 	}
// 
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void ecsUpdateBullet::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& /*_singletonComponents*/
// 							   , ComponentData< ecsGameobject >& _gameobjects
// 							   , ComponentData< ecsBullet >& _bullets )
// 	{
// 		for ( int entity = 0; entity < _count; entity++ )
// 		{
// 			ecsComponentsKey& key = _entitiesData[ entity ];
// 
// 			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 			{
// 				ecsBullet& bullet = _bullets.At( key );
// 
// 				bullet.durationLeft -= _delta;
// 				if ( bullet.durationLeft <= 0.f )
// 				{
// 					Gameobject* gameobject = _gameobjects.At( key ).gameobject;
// 					gameobject->GetScene().DeleteGameobject( gameobject );
// 				}
// 			}
// 		}
//	}
}