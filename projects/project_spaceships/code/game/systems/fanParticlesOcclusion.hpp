#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanParticle.hpp"
#include "game/singletons/fanSunLight.hpp"
#include "game/fanGameTags.hpp"

namespace fan
{
	//========================================================================================================
	// removes the particles that are not in the sunlight
	//========================================================================================================
	struct SParticlesOcclusion : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature< Particle >() |
				_world.GetSignature<TagSunlightOcclusion>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			SunLight& sunlight = _world.GetSingleton<SunLight>();

            for( auto particleIt = _view.begin<Particle>();
                 particleIt != _view.end<Particle>();
                 ++particleIt )
            {
				const EcsEntity entity = particleIt.GetEntity();
				const Particle& particle = *particleIt;

				const btVector3& position = ToBullet( particle.mPosition );

				// raycast on the light mesh
				const btVector3 rayOrigin = btVector3( position[0], 1.f, position[2] );
				btVector3 outIntersection;
                bool isInsideSunlight = sunlight.mMesh->RayCast( rayOrigin,
                                                                 -btVector3_Up,
                                                                 outIntersection );
				if( !isInsideSunlight )
				{
					_world.Kill( entity );
				}
			}
		}
	};
}