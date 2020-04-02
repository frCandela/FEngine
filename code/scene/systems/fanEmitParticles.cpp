#include "scene/systems/fanEmitParticles.hpp"

#include "core/fanRandom.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanParticle.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_EmitParticles::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>() | _world.GetSignature<ParticleEmitter>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_EmitParticles::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EntityID id : _entities )
		{
			const Transform& emitterTransform = _world.GetComponent<Transform>( id );
			ParticleEmitter& emitter = _world.GetComponent<ParticleEmitter>( id );

			if( emitter.particlesPerSecond > 0.f && emitter.enabled )
			{
				emitter.timeAccumulator += _delta;
				float particleSpawnDelta = 1.f / emitter.particlesPerSecond;

				const glm::vec3 origin = ToGLM( emitterTransform.GetPosition() );
				const glm::vec3 transformedOffset = ToGLM( emitterTransform.TransformDirection( emitter.offset ) );

				// Spawn particles
				while( emitter.timeAccumulator > particleSpawnDelta )
				{
					emitter.timeAccumulator -= particleSpawnDelta;

					EntityID entity = _world.CreateEntity();
					Particle& particle = _world.AddComponent<Particle>( entity );
					//if( m_sunlightParticleOcclusionActive ) { m_ecsManager->AddComponent<ecsSunlightParticleOcclusion>( entity ); } @hack

					particle.speed = glm::normalize( glm::vec3( Random::FloatClip(), Random::FloatClip(), Random::FloatClip() ) );
					particle.speed *= emitter.speed;
					particle.position = origin + transformedOffset;
					particle.durationLeft = emitter.duration;
					particle.color = emitter.color;
				}
			}
		}
	}
}