#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanParticle.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Moves particles, kill them when they are out of time
	//==============================================================================================================================================================
	struct S_UpdateParticles : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Particle>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }
			SCOPED_PROFILE( UpdateParticles );

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
	};
}