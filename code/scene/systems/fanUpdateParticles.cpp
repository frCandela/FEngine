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
	void S_UpdateParticles::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( auto particleIt = _view.begin<Particle>(); particleIt != _view.end<Particle>(); ++particleIt )
		{
			const EcsEntity entity = particleIt.Entity();
			Particle& particle = *particleIt;

			particle.durationLeft -= _delta;
			if( particle.durationLeft < 0.f )
			{
				_world.Kill( entity );
			}
			particle.position += _delta * particle.speed;
		}
	}
}