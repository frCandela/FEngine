#include "scene/systems/fanUpdateParticles.hpp"

#include "scene/components/fanParticle.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateParticles::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Particle>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateParticles::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EcsEntity entity : _entities )
		{
			Particle& particle = _world.GetComponent<Particle>( entity );

			particle.durationLeft -= _delta;
			if( particle.durationLeft < 0.f )
			{
				_world.KillEntity( entity );
			}
			particle.position += _delta * particle.speed;
		}
	}
}