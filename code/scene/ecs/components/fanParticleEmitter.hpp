#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	// Emits particles 
	// timeAccumulator is used to keep track of time to spawn particles with proper timing
	//==============================================================================================================================================================
	struct ParticleEmitter : public ecComponent
	{
		DECLARE_COMPONENT( ParticleEmitter )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _emitter );
		static void Save( const ecComponent& _emitter, Json& _json );
		static void Load( ecComponent& _emitter, const Json& _json );

		int				particlesPerSecond;
		float			speed;
		float			duration;
		btVector3		offset;
		Color			color;

		float			timeAccumulator;
	};
	static constexpr size_t sizeof_particleEmitter = sizeof( ParticleEmitter );
}