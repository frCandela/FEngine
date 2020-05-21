#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Emits particles 
	// timeAccumulator is used to keep track of time to spawn particles with proper timing
	//==============================================================================================================================================================
	struct ParticleEmitter : public EcsComponent
	{
		ECS_COMPONENT( ParticleEmitter )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		bool		enabled;
		float		particlesPerSecond;
		float		speed;					// speed of the particles
		float		duration;				// lifetime of a particle in seconds
		Color		color;
		Signature	tagsSignature;					// tags added to the particles

		float		timeAccumulator;
	};
	static constexpr size_t sizeof_particleEmitter = sizeof( ParticleEmitter );
}