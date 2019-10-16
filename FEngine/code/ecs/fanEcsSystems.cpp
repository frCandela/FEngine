#include "fanGlobalIncludes.h"
#include "ecs/fanECSSystems.h"

#include "renderer/fanRenderer.h"
#include "core/fanTime.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsPosition > & _positions,
		std::vector< ecsRotation > & /*_rotations*/,
		std::vector< ecsMovement > & _movements,
		std::vector< ecsParticle > & _particles ) {

		std::vector<btVector3> triangles;
		std::vector<Color> colors;
		triangles.reserve( _entitiesData.size() );
		colors.reserve( _entitiesData.size() );

		static const float size = 0.05f;

		for ( int entity = 0; entity < _count; entity++ ) {
			ecsEntityData & data = _entitiesData[entity];

			if ( data.IsAlive() && ( data.bitset & ParticleSystem::signature::bitset ) == ParticleSystem::signature::bitset ) {
				btVector3& position = _positions[data.components[IndexOfComponent<ecsPosition>::value]].position;
				ecsMovement& movement = _movements[data.components[IndexOfComponent<ecsMovement>::value]];
				ecsParticle& particle = _particles[data.components[IndexOfComponent<ecsParticle>::value]];

				(void)particle;

				particle.durationLeft -= _delta;
				if ( particle.durationLeft < 0 ) {
					data.Kill();
				}
				position += _delta * movement.speed;


				triangles.push_back( position + btVector3( size, 0, 0 ) );
				triangles.push_back( position + btVector3( -size, 0, 0 ) );
				triangles.push_back( position + btVector3( 0, 2.f*size, 0 ) );
				colors.push_back( particle.color );
			}
		}

		Debug::Render().DebugTriangles( triangles, colors );
	}


	//================================================================================================================================
	//================================================================================================================================
	void PlanetsSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
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
	void SynchRbToTransSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
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
				motionState.Get().setWorldTransform( transform );
				(void)motionState; (void)rigidbody; (void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SynchTransToRbSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
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
}