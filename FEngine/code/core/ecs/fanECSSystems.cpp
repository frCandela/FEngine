#include "fanGlobalIncludes.h"
#include "core/ecs/fanEcsSystems.h"

#include "renderer/fanRenderer.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	void ParticleSystem::Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
		std::vector< ecsTranform > & _transforms,
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
				ecsTranform& transform = _transforms[data.components[IndexOfComponent<ecsTranform>::value]];
				ecsMovement& movement = _movements  [data.components[IndexOfComponent<ecsMovement>::value]];
				ecsParticle& particle = _particles  [data.components[IndexOfComponent<ecsParticle>::value]];

				(void)particle;

				particle.durationLeft -= _delta;
				if ( particle.durationLeft < 0 ) {
					data.Kill();
				}
				transform.position += _delta * movement.speed;


				triangles.push_back( transform.position + btVector3( size, 0, 0 ) );
				triangles.push_back( transform.position + btVector3( -size, 0, 0 ) );
				triangles.push_back( transform.position + btVector3( 0, 2.f*size, 0 ) );
				colors.push_back( particle.color );
			}
		}

		Debug::Render().DebugTriangles( triangles, colors );
	}
}