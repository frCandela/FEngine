#include "core/ecs/fanEcsSystem.hpp"
#include "core/random/fanRandom.hpp"
#include "engine/physics/fanFxTransform.hpp"
#include "engine/components/fanParticleEmitter.hpp"
#include "engine/components/fanParticle.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Spawns particles for all particle emitters in the scene
    //==================================================================================================================================================================================================
    struct SEmitParticles : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() | _world.GetSignature<ParticleEmitter>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
        {
            SCOPED_PROFILE( S_EmitParticles );

            if( _delta == 0.f ){ return; }

            auto transformIt       = _view.begin<FxTransform>();
            auto particleEmitterIt = _view.begin<ParticleEmitter>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++particleEmitterIt )
            {
                const FxTransform& emitterTransform = *transformIt;
                ParticleEmitter  & emitter          = *particleEmitterIt;

                if( emitter.mParticlesPerSecond > 0.f && emitter.mEnabled )
                {
                    emitter.mTimeAccumulator += _delta;
                    float particleSpawnDelta = 1.f / emitter.mParticlesPerSecond;

                    const glm::vec3 origin = emitterTransform.mPosition.ToGlm();

                    // Spawn particles
                    while( emitter.mTimeAccumulator > particleSpawnDelta )
                    {
                        emitter.mTimeAccumulator -= particleSpawnDelta;

                        EcsEntity entity = _world.CreateEntity();
                        Particle& particle = _world.AddComponent<Particle>( entity );

                        particle.mSpeed        = glm::normalize( glm::vec3( Random::FloatClip(),
                                                                            Random::FloatClip(),
                                                                            Random::FloatClip() ) );
                        particle.mSpeed *= emitter.mSpeed;
                        particle.mPosition     = origin;
                        particle.mDurationLeft = emitter.mDuration;
                        particle.mColor        = emitter.mColor;

                        _world.AddTagsFromSignature( entity, emitter.mTagsSignature );
                    }
                }
            }
        }
    };
}