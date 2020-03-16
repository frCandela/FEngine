#include "scene/ecs/fanSystem.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

#include "scene/ecs/components/fanParticle.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Moves particles, kill them when they are out of time
	//==============================================================================================================================================================
	struct S_UpdateParticles : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Particle>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID entityID : _entities )
			{
				Particle& particle = _world.GetComponent<Particle>( entityID );

				particle.durationLeft -= _delta;
				if( particle.durationLeft < 0.f )
				{
					_world.KillEntity(entityID);
				}
				particle.position += _delta * particle.speed;
			}
		}
	};
}