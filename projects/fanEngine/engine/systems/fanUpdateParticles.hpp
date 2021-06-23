#include "ecs/fanEcsSystem.hpp"
#include "engine/components/fanParticle.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Moves particles, kill them when they are out of time
    //==================================================================================================================================================================================================
    struct SUpdateParticles : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Particle>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
        {
            if( _delta == 0.f ){ return; }
            SCOPED_PROFILE( UpdateParticles );

            for( auto particleIt = _view.begin<Particle>(); particleIt != _view.end<Particle>(); ++particleIt )
            {
                const EcsEntity entity = particleIt.GetEntity();
                Particle& particle = *particleIt;

                particle.mDurationLeft -= _delta;
                if( particle.mDurationLeft < 0.f )
                {
                    _world.Kill( entity );
                }
                particle.mPosition += _delta * particle.mSpeed;
            }
        }
    };
}