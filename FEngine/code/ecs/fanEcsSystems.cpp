#include "fanGlobalIncludes.h"
#include "ecs/fanECSSystems.h"

#include "renderer/fanRendererDebug.h"
#include "core/time/fanTime.h"
#include "core/time/fanProfiler.h"
#include "core/math/shapes/fanConvexHull.h"
#include "renderer/fanMesh.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	void ecsParticleSystem::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
		ComponentData< ecsPosition > & _positions,
		ComponentData< ecsRotation > & /*_rotations*/,
		ComponentData< ecsMovement > & _movements,
		ComponentData< ecsParticle > & _particles ) 
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) ) {
				btVector3& position = _positions.At(key).position;
				ecsMovement& movement = _movements.At( key );
				ecsParticle& particle = _particles.At( key );

				particle.durationLeft -= _delta;
				if ( particle.durationLeft < 0 ) {
					key.Kill();
				}
				position += _delta * movement.speed;				
			}
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	btVector3 ecsParticlesGenerateSystem::s_cameraPosition;
	void ecsParticlesGenerateSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsPosition > & _positions
		, ComponentData< ecsParticle > & _particles )
	{
		std::vector<btVector3> triangles;
		std::vector<Color> colors;
		triangles.reserve( _entitiesData.size() );
		colors.reserve( _entitiesData.size() );

		const btVector3 up = btVector3::Left();
		static const float size = 0.05f;

		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				btVector3& position = _positions.At( key ).position;
				ecsParticle& particle = _particles.At( key );

				btVector3 forward = s_cameraPosition - position;
				forward.normalize();
				btVector3 left = up.cross( forward );
				left.normalize();

				btMatrix3x3 mat (
					left[0], up[0], forward[0],
					left[1], up[1], forward[1],
					left[2], up[2], forward[2] );

				triangles.push_back( position + mat * btVector3( size, 0, 0 ) );
				triangles.push_back( position + mat * btVector3( -size, 0, 0 ) );
				triangles.push_back( position + mat * btVector3( 0, 2.f*size, 0 ) );
				colors.push_back( particle.color );
			}
		}

		Debug::Render().DebugTriangles( triangles, colors );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsParticleSunlightOcclusionSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsPosition > & _positions
		, ComponentData< ecsParticle > & /*_particles*/
		, ComponentData< ecsSunlightParticleOcclusion > & /*_occlusion*/ )
	{
		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				// get data
				btVector3& position = _positions.At( key ).position;

				// raycast on the light mesh
				const btVector3 rayOrigin = btVector3(position[0], 1.f, position[2] );
				btVector3 outIntersection;
				bool isInsideSunlight = ecsSolarEruptionMeshSystem::s_mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );
				if ( ! isInsideSunlight )
				{
					key.Kill();
				}
			}
		}


	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsPlanetsSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		,ComponentData< ecsGameobject > & _gameobjects
		,ComponentData< ecsTranform > & _transforms
		,ComponentData< ecsPlanet > &	_planets
		,ComponentData< ecsFlags > &	_flags )
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() &&  key.MatchSignature( signature::bitset ) ) {
				btTransform& transform		= _transforms.At(key).transform;
				ecsPlanet& planet			= _planets.At(key);
				const btTransform& parentTransform = _gameobjects.At( key ).gameobject->GetParent()->GetTransform()->GetBtTransform();
				ecsFlags& flags = _flags.At( key );

				float const time = -planet.speed * Time::ElapsedSinceStartup();
				btVector3 position( std::cosf( time + planet.phase ), 0, std::sinf( time + planet.phase ) );

				transform.setOrigin( parentTransform.getOrigin() + planet.radius * position);
				flags.flags |= ecsFlags::OUTDATED_AABB;
				flags.flags |= ecsFlags::OUTDATED_TRANSFORM;
			}
		}
	}

	//================================================================================================================================
	// Helper : Generates a triangle that represents a segment of a circle of radius m_radius
	//================================================================================================================================
	void ecsSolarEruptionMeshSystem::AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 )
	{
		const glm::vec3 normal( 0.f, 1.f, 0.f );
		const glm::vec3 color( 1.f, 1.f, 1.f );
		const glm::vec3 center( 0.f, 0.f, 0.f );
		const glm::vec2 centerUV( 0.5f, 0.5f );

		glm::vec3 p1( _v0[0], 0.f, _v0[2] );
		glm::vec3 p2( _v1[0], 0.f, _v1[2] );
		glm::vec2 uv1( _v0[0], _v0[2] );
		glm::vec2 uv2( _v1[0], _v1[2] );

		uv1 = 0.5f * uv1 / s_radius + glm::vec2( 0.5f, 0.5f );
		uv2 = 0.5f * uv2 / s_radius + glm::vec2( 0.5f, 0.5f );

		_vertices.push_back( { center,	normal, color,centerUV } );
		_vertices.push_back( { p1,		normal, color, uv1 } );
		_vertices.push_back( { p2,		normal, color, uv2 } );


		if ( s_debugDraw )
		{

			const Color debugColor( 1.f, 1.f, 0.f, 0.3f );
			Debug::Render().DebugTriangle( btVector3::Zero(), _v0, _v1, debugColor );
			Debug::Render().DebugLine( btVector3::Zero(), _v0, Color::Green );
		}
	}




	//================================================================================================================================
	//================================================================================================================================
	float  ecsSolarEruptionMeshSystem::s_subAngle = 45.f;
	float  ecsSolarEruptionMeshSystem::s_radius = 100.f;
	bool   ecsSolarEruptionMeshSystem::s_debugDraw = false;
	Mesh*  ecsSolarEruptionMeshSystem::s_mesh = new Mesh();

	//================================================================================================================================
	//================================================================================================================================
	void ecsSolarEruptionMeshSystem::Init()
	{
		s_mesh->SetHostVisible( true );
		s_mesh->SetOptimizeVertices( false );
		s_mesh->SetAutoUpdateHull( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsSolarEruptionMeshSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsPlanet > &	/*_planets*/
		, ComponentData< ecsTranform > & _transforms 
		, ComponentData< ecsScaling > & _scaling )

	{
		SCOPED_PROFILE( ecs_solar_erup )

		// Get the interesting matching keys
		std::vector<ecsComponentsKey*> interestingKeys;
		interestingKeys.reserve( 64 );
		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				interestingKeys.push_back(&key );
			}
		}

		// Generates occlusion rays for each planet
		std::vector< OrientedSegment > segments;
		segments.reserve( 2 * interestingKeys.size() );
		for (int keyIndex = 0; keyIndex < interestingKeys.size(); keyIndex++)
		{
			ecsComponentsKey & key = *interestingKeys[keyIndex];
			btTransform& transform = _transforms.At( key ).transform;
			btVector3& scale = _scaling.At( key ).scale;

			const btVector3 planetPos = transform.getOrigin();
			const btVector3 direction = planetPos - btVector3::Zero();
			const btVector3 left = btVector3::Up().cross( direction ).normalized();

			const btVector3 leftDirection = scale.getX() * left;
			const btVector3 planetLeft = planetPos + leftDirection;
			const btVector3 planetRight = planetPos - leftDirection;
			const float norm = planetRight.norm();

			segments.push_back( { planetRight, OrientedSegment::RIGHT, norm } );
			segments.push_back( { planetLeft, OrientedSegment::LEFT, norm } );
		}

		// Sort the segments in ascending order ( counter clockwise )
		std::sort( std::begin( segments ), std::end( segments ),
			[] ( OrientedSegment& _s1, OrientedSegment& _s2 )
		{
			return btVector3::Left().SignedAngle( _s1.direction, btVector3::Up() ) < btVector3::Left().SignedAngle( _s2.direction, btVector3::Up() );
		} );

		// Finds the starting point of mesh generation loop
		int startIndex = 0;
		{
			// Finds the index of a RIGHT axis that has a minimal number of nested levels of planets	(depth)	
			int startIndexDepth = 10000;
			int depth = 0;	//			
			for ( int axisIndex = 0; axisIndex < segments.size(); axisIndex++ )
			{
				OrientedSegment& axis = segments[axisIndex];
				depth += axis.openSide == OrientedSegment::RIGHT ? 1 : -1;
				if ( axis.openSide == OrientedSegment::RIGHT && depth < startIndexDepth )
				{
					startIndex = axisIndex;
					startIndexDepth = depth;
				}
			}
		}

		// generates the mesh
		std::vector<Vertex>	vertices;
		vertices.reserve( 3 * interestingKeys.size() );
		const float minGapRadians = btRadians( s_subAngle );
		std::set<float> norms;	// Stores the nested opening segments norms
		for ( int axisIndex = 0; axisIndex < segments.size(); axisIndex++ )
		{
			const int index = ( axisIndex + startIndex ) % segments.size();
			const int indexNext = ( index + 1 ) % segments.size();
			OrientedSegment& axis = segments[index];
			OrientedSegment& axisNext = segments[indexNext];

			float length = 0.f;
			if ( axis.openSide == OrientedSegment::RIGHT )
			{
				// Easy case of an opening segment
				norms.insert( axis.norm );
				length = *norms.begin(); // Gets the smallest norm
			}
			else if ( axis.openSide == OrientedSegment::LEFT )
			{
				norms.erase( axis.norm );
				if ( !norms.empty() )
				{
					length = *norms.begin(); // Easy case of a closing segment
				}
				else
				{
					if ( axisNext.openSide == OrientedSegment::RIGHT )
					{
						// Empty space with no planets -> fills the space with triangles
						float angle = axis.direction.SignedAngle( axisNext.direction, btVector3::Up() );
						if ( angle > minGapRadians ) // gap is too large -> subdivise it
						{

							const int numSubdivistions = int( angle / minGapRadians ) + 1;
							const float subdivisionAngle = angle / numSubdivistions;
							btTransform rotate( btQuaternion( btVector3::Up(), subdivisionAngle ) );
							btVector3 subAxisNext = s_radius * axis.direction / axis.norm;
							for ( int subAxisIndex = 0; subAxisIndex < numSubdivistions; subAxisIndex++ )
							{
								btVector3 subAxis = subAxisNext;
								subAxisNext = rotate * subAxisNext;
								AddSunTriangle( vertices, subAxis, subAxisNext );
							}
						}
						else // gap size is small enough
						{
							length = s_radius;
						}
					}
					else
					{
						// I think this is a degenerate case that doesn't need to be considered #provemewrong
						////Debug::Warning( "VIDE JURIDIQUE" );
					}
				}
			}

			// Generate a light triangle based on the processed segment length
			if ( length > 0.f )
			{
				const btVector3 v0 = length * axis.direction / axis.norm;
				const btVector3 v1 = length * axisNext.direction / axisNext.norm;
				AddSunTriangle( vertices, v0, v1 );
			}
		}

		// Load mesh
		s_mesh->LoadFromVertices( vertices );
	}


	//================================================================================================================================
	//================================================================================================================================
	void ecsSynchRbSystem::SynchRbToTransSystem( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		,ComponentData< ecsTranform > & _transforms
		,ComponentData< ecsMotionState > & _motionStates
		,ComponentData< ecsRigidbody > & _rigidbodies ) 
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				btTransform& transform =	_transforms.At(key).transform;
				ecsMotionState& motionState = _motionStates.At(key);
				ecsRigidbody& rigidbody = _rigidbodies.At(key);

				rigidbody.Get().setWorldTransform( transform );
				motionState.Get().setWorldTransform( transform );
				(void)motionState; (void)rigidbody; (void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsSynchRbSystem::SynchTransToRbSystem( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsTranform > &    _transforms
		, ComponentData< ecsMotionState > & _motionStates
		, ComponentData< ecsRigidbody > &   _rigidbodies ) 
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) ) {
				btRigidBody&  rigidbody = _rigidbodies.At( key ).Get();
				if( rigidbody.getInvMass() <= 0.f ){ continue; }

				btTransform& transform		= _transforms.At(key).transform;
				ecsMotionState& motionState = _motionStates.At(key);
				

				btMotionState * ms = rigidbody.getMotionState();
				ms->getWorldTransform( transform );
				(void)motionState;(void )rigidbody;(void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromHull::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsTranform > &	_transforms
		, ComponentData< ecsScaling > &		_scales
		, ComponentData< ecsAABB > &		_aabbs
		, ComponentData< ecsFlags > &		_flags
		, ComponentData< ecsMesh >&			_mesh )
	{
		// Find all interesting entities
		std::vector< ecsComponentsKey * > usefullEntitiesKeys;
		usefullEntitiesKeys.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ ) 
		{
			ecsComponentsKey & key = _entitiesData[entity];			
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				uint32_t&			flags = _flags.At(key).flags;
				const ConvexHull&	hull  = _mesh.At(key).mesh->GetHull();
				if( flags & ecsFlags::OUTDATED_AABB && ! ( flags & ecsFlags::NO_AABB_UPDATE ) && ! hull.IsEmpty() )
				{
					usefullEntitiesKeys.push_back( &key );
					flags &= ~ecsFlags::OUTDATED_AABB;
				}
			}
		}

		// Calculates model matrices
		std::vector< glm::mat4 > modelMatrices;
		modelMatrices.reserve( usefullEntitiesKeys.size() );
		for ( int dataIndex = 0; dataIndex < usefullEntitiesKeys.size(); dataIndex++ ) {
			ecsComponentsKey & key = *usefullEntitiesKeys[dataIndex];

			btTransform& transform  = _transforms.At(key).transform;
			btVector3&	 btscale	= _scales.At(key).scale;

			const glm::vec3 position	= ToGLM( transform.getOrigin() );
			const glm::vec3 scale		= ToGLM( btscale );
			const glm::quat rotation	= ToGLM( transform.getRotation() );

			modelMatrices.push_back(
				glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), scale )
			);
		}

		// Calculates the new aabb using the model matrix and the convex hull
		for ( int dataIndex = 0; dataIndex < usefullEntitiesKeys.size(); dataIndex++ ) {
			ecsComponentsKey & key = *usefullEntitiesKeys[dataIndex];
			glm::mat4&			modelMatrix = modelMatrices[dataIndex];
			const ConvexHull&	hull = _mesh.At( key ).mesh->GetHull();
			AABB&				aabb = _aabbs.At(key).aabb;
			aabb = AABB( hull.GetVertices(), modelMatrix );
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromRigidbody::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
		  ComponentData< ecsAABB > &		_aabbs
		, ComponentData< ecsRigidbody > &	_rigidbodies
		, ComponentData< ecsFlags >    &	_flags
	)
	{
		// Find all interesting entities
		std::vector< ecsComponentsKey * > outdatedAABBEntities;
		outdatedAABBEntities.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				uint32_t& flags = _flags.At( key ).flags;
				if ( flags & ecsFlags::OUTDATED_AABB && !( flags & ecsFlags::NO_AABB_UPDATE ) )
				{
					outdatedAABBEntities.push_back( &key );
					flags &= ~ecsFlags::OUTDATED_AABB;
				}
			}
		}

		// Update aabb
		for ( int dataIndex = 0; dataIndex < outdatedAABBEntities.size(); dataIndex++ )
		{
			ecsComponentsKey & key = *outdatedAABBEntities[dataIndex];

			btRigidBody& rb = _rigidbodies.At( key ).Get();
			AABB& aabb = _aabbs.At( key ).aabb;
			btVector3 low, high;
			rb.getAabb(low, high);
			aabb = AABB( low, high );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromTransform::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsTranform > &	_transforms
		, ComponentData< ecsAABB > &		_aabbs
		, ComponentData< ecsFlags > &		_flags ) {
		// Find all interesting entities
		std::vector< ecsComponentsKey * > usefullData;
		usefullData.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ ) 
		{
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				uint32_t& flags = _flags.At( key ).flags;
				if ( flags & ecsFlags::OUTDATED_AABB && !( flags & ecsFlags::NO_AABB_UPDATE ) ) {
					usefullData.push_back( &key );
					flags &= ~ecsFlags::OUTDATED_AABB;
				}
			}
		}

		// Update aabb
		std::vector< glm::mat4 > modelMatrices;
		modelMatrices.reserve( usefullData .size() );
		for (int dataIndex = 0; dataIndex < usefullData.size(); dataIndex++){
			ecsComponentsKey & key = *usefullData[dataIndex];
		
			btTransform& transform	= _transforms.At(key).transform;
			AABB& aabb				= _aabbs.At(key).aabb;
			const btVector3 origin = transform.getOrigin();
				const float size = 0.05f;
				aabb = AABB( origin - size * btVector3::One(), origin + size * btVector3::One() );							
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateBullet::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
		ComponentData< ecsGameobject > &	_gameobjects
		, ComponentData< ecsBullet >     &	_bullets )
	{
		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				ecsBullet& bullet =		 _bullets.At( key );

				bullet.durationLeft -= _delta;
				if ( bullet.durationLeft <= 0.f )
				{
 					Gameobject * gameobject = _gameobjects.At( key ).gameobject;
					gameobject->GetScene()->DeleteGameobject( gameobject );
				}
			}
		}
	}
}