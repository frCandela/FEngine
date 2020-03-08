#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEntity.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct ParticleEmitter : public ecComponent
	{
		DECLARE_COMPONENT( ParticleEmitter )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Clear( ecComponent& _emitter );
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