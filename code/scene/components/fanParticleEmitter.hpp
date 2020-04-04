#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Emits particles 
	// timeAccumulator is used to keep track of time to spawn particles with proper timing
	//==============================================================================================================================================================
	struct ParticleEmitter : public Component
	{
		DECLARE_COMPONENT( ParticleEmitter )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		bool		enabled;
		float		particlesPerSecond;
		float		speed;
		float		duration;
		btVector3	offset;
		Color		color;

		float		timeAccumulator;
	};
	static constexpr size_t sizeof_particleEmitter = sizeof( ParticleEmitter );
}