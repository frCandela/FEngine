#include "game/systems/fanParticlesOcclusion.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanParticle.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
#include "game/fanGameTags.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_ParticlesOcclusion::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature< Particle >() |
			_world.GetSignature< tag_sunlight_occlusion>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_ParticlesOcclusion::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		SunLight& sunlight = _world.GetSingletonComponent<SunLight>();

		for( EntityID entityID : _entities )
		{
			const Particle& particle = _world.GetComponent<Particle>( entityID );
			const btVector3& position = ToBullet( particle.position );

			// raycast on the light mesh
			const btVector3 rayOrigin = btVector3( position[0], 1.f, position[2] );
			btVector3 outIntersection;
			bool isInsideSunlight = sunlight.mesh.RayCast( rayOrigin, -btVector3::Up(), outIntersection );
			if( !isInsideSunlight )
			{
				_world.KillEntity( entityID );
			}
		}

	}
}