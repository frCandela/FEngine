#include "fanGlobalIncludes.h"
#include "ecs/fanECSSystems.h"

#include "renderer/fanRenderer.h"
#include "core/time/fanTime.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	btVector3 ecsParticleSystem::s_cameraPosition;
	void ecsParticleSystem::Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsPosition > & _positions,
		std::vector< ecsRotation > & /*_rotations*/,
		std::vector< ecsMovement > & _movements,
		std::vector< ecsParticle > & _particles ) {

		std::vector<btVector3> triangles;
		std::vector<Color> colors;
		triangles.reserve( _entitiesData.size() );
		colors.reserve( _entitiesData.size() );

		const btVector3 up = btVector3::Left();
		static const float size = 0.05f;

		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];

			if ( data.IsAlive() && ( data.bitset & ecsParticleSystem::signature::bitset ) == ecsParticleSystem::signature::bitset ) {
				btVector3& position = _positions[data.components[IndexOfComponent<ecsPosition>::value]].position;
				ecsMovement& movement = _movements[data.components[IndexOfComponent<ecsMovement>::value]];
				ecsParticle& particle = _particles[data.components[IndexOfComponent<ecsParticle>::value]];

				(void)particle;

				particle.durationLeft -= _delta;
				if ( particle.durationLeft < 0 ) {
					data.Kill();
				}
				position += _delta * movement.speed;

				
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
	void ecsPlanetsSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsTranform > & _transforms,
		std::vector< ecsPlanet > & _planets ) 	
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];
			if ( data.IsAlive() && ( data.bitset & signature::bitset ) == signature::bitset ) {
				btTransform& transform		= _transforms[data.components[IndexOfComponent<ecsTranform>::value]].transform;
				ecsPlanet& planet			= _planets[data.components[IndexOfComponent<ecsPlanet>::value]];
				btTransform& parentTransform = _transforms[_entitiesData[planet.parentEntity].components[IndexOfComponent<ecsTranform>::value]].transform;

				float const time = -planet.speed * Time::ElapsedSinceStartup();
				btVector3 position( std::cosf( time + planet.phase ), 0, std::sinf( time + planet.phase ) );

				transform.setOrigin( parentTransform.getOrigin() + planet.radius * position);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsSynchRbToTransSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsTranform > & _transforms
		, std::vector< ecsMotionState > & _motionStates
		, std::vector< ecsRigidbody > & _rigidbodies ) 
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];

			if ( data.IsAlive() && ( data.bitset & signature::bitset ) == signature::bitset )
			{
				btTransform& transform = _transforms[data.components[IndexOfComponent<ecsTranform>::value]].transform;
				ecsMotionState& motionState = _motionStates[data.components[IndexOfComponent<ecsMotionState>::value]];
				ecsRigidbody& rigidbody = _rigidbodies[data.components[IndexOfComponent<ecsRigidbody>::value]];

				rigidbody.Get().setWorldTransform( transform );
				//motionState.Get().setWorldTransform( transform );
				(void)motionState; (void)rigidbody; (void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsSynchTransToRbSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsTranform > & _transforms
		, std::vector< ecsMotionState > & _motionStates
		, std::vector< ecsRigidbody > & _rigidbodies ) {
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];

			if ( data.IsAlive() && ( data.bitset & signature::bitset ) == signature::bitset ) {
				btTransform& transform = _transforms[data.components[IndexOfComponent<ecsTranform>::value]].transform;
				ecsMotionState& motionState = _motionStates[data.components[IndexOfComponent<ecsMotionState>::value]];
				ecsRigidbody& rigidbody = _rigidbodies[data.components[IndexOfComponent<ecsRigidbody>::value]];

				btMotionState * ms = rigidbody.Get().getMotionState();
				ms->getWorldTransform( transform );
				(void)motionState;(void )rigidbody;(void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromHull::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsTranform > &	_transforms
		, std::vector< ecsScaling > &	_scales
		, std::vector< ecsAABB > &		_aabbs
		, std::vector< ecsFlags > &		_flags
		, std::vector< ecsConvexHull >& _hulls )
	{
		// Find all interesting entities
		std::vector< ecsEntityData * > usefullData;
		usefullData.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];			
			if ( data.IsAlive() && ( data.bitset & signature::bitset ) == signature::bitset ) {
				uint32_t& flags = _flags[data.components[IndexOfComponent<ecsFlags>::value]].flags;
				ConvexHull&		hull = _hulls[data.components[IndexOfComponent<ecsConvexHull>::value]].convexHull;
				if( flags & ecsFlags::OUTDATED_TRANSFORM && ! hull.IsEmpty() ) {
					usefullData.push_back( &data );
					flags &= ~ecsFlags::OUTDATED_TRANSFORM;
				}
			}
		}

		// Calculates model matrices
		std::vector< glm::mat4 > modelMatrices;
		modelMatrices.reserve( usefullData.size() );
		for ( int dataIndex = 0; dataIndex < usefullData.size(); dataIndex++ ) {
			ecsEntityData & data = *usefullData[dataIndex];

			btTransform& transform = _transforms[data.components[IndexOfComponent<ecsTranform>::value]].transform;
			btVector3&	 btscale		= _scales[data.components[IndexOfComponent<ecsScaling>::value]].scale;

			const glm::vec3 position	= ToGLM( transform.getOrigin() );
			const glm::vec3 scale		= ToGLM( btscale );
			const glm::quat rotation	= ToGLM( transform.getRotation() );

			modelMatrices.push_back(
				glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), scale )
			);
		}

		// Calculates the new aabb using the model matrix and the convex hull
		for ( int dataIndex = 0; dataIndex < usefullData.size(); dataIndex++ ) {
			ecsEntityData & data = *usefullData[dataIndex];
			glm::mat4&		modelMatrix = modelMatrices[dataIndex];
			ConvexHull&		hull		= _hulls[data.components[IndexOfComponent<ecsConvexHull>::value]].convexHull;
			AABB&			aabb		= _aabbs[data.components[IndexOfComponent<ecsAABB>::value]].aabb;
			aabb = AABB( hull.GetVertices(), modelMatrix );
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromTransform::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsTranform > &	_transforms
		, std::vector< ecsAABB > &		_aabbs
		, std::vector< ecsFlags > &		_flags ) {
		// Find all interesting entities
		std::vector< ecsEntityData * > usefullData;
		usefullData.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];
			if ( data.IsAlive() && ( data.bitset & signature::bitset ) == signature::bitset ) {
				uint32_t& flags = _flags[data.components[IndexOfComponent<ecsFlags>::value]].flags;
				if ( flags & ecsFlags::OUTDATED_TRANSFORM ) {
					usefullData.push_back( &data );
					flags &= ~ecsFlags::OUTDATED_TRANSFORM;
				}
			}
		}

		// Update aabb
		std::vector< glm::mat4 > modelMatrices;
		modelMatrices.reserve( usefullData .size() );
		for (int dataIndex = 0; dataIndex < usefullData.size(); dataIndex++){
			ecsEntityData & data = *usefullData[dataIndex];
		
			btTransform& transform	= _transforms[data.components[IndexOfComponent<ecsTranform>::value]].transform;
			AABB& aabb				= _aabbs[data.components[IndexOfComponent<ecsAABB>::value]].aabb;
			const btVector3 origin = transform.getOrigin();
				const float size = 0.05f;
				aabb = AABB( origin - size * btVector3::One(), origin + size * btVector3::One() );							
		}
	}
}