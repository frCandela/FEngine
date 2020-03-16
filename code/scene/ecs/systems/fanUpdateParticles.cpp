#include "scene/ecs/systems/fanUpdateParticles.hpp"

#include "scene/ecs/components/fanParticle.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateParticles::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Particle>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateParticles::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID entityID : _entities )
		{
			Particle& particle = _world.GetComponent<Particle>( entityID );

			particle.durationLeft -= _delta;
			if( particle.durationLeft < 0.f )
			{
				_world.KillEntity( entityID );
			}
			particle.position += _delta * particle.speed;
		}
	}
}