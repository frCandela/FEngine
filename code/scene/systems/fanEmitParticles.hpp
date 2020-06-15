#include "ecs/fanEcsSystem.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/fanRandom.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanParticle.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Spawns particles for all particle emitters in the scene
	//==============================================================================================================================================================
	struct S_EmitParticles : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Transform>() | _world.GetSignature<ParticleEmitter>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			SCOPED_PROFILE( S_EmitParticles );

			if( _delta == 0.f ) { return; }

			auto transformIt = _view.begin<Transform>();
			auto particleEmitterIt = _view.begin<ParticleEmitter>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++particleEmitterIt )
			{
				const Transform& emitterTransform = *transformIt;
				ParticleEmitter& emitter = *particleEmitterIt;

				if( emitter.particlesPerSecond > 0.f && emitter.enabled )
				{
					emitter.timeAccumulator += _delta;
					float particleSpawnDelta = 1.f / emitter.particlesPerSecond;

					const glm::vec3 origin = ToGLM( emitterTransform.GetPosition() );

					// Spawn particles
					while( emitter.timeAccumulator > particleSpawnDelta )
					{
						emitter.timeAccumulator -= particleSpawnDelta;

						EcsEntity entity = _world.CreateEntity();
						Particle& particle = _world.AddComponent<Particle>( entity );

						particle.speed = glm::normalize( glm::vec3( Random::FloatClip(), Random::FloatClip(), Random::FloatClip() ) );
						particle.speed *= emitter.speed;
						particle.position = origin;
						particle.durationLeft = emitter.duration;
						particle.color = emitter.color;

						_world.AddTagsFromSignature( entity, emitter.tagsSignature );
					}
				}
			}
		}
	};
}