#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanPointLight.hpp"

namespace fan
{
	//================================================================================================================================
	// SolarEruption is the fx of the sun exploding out and projecting matter all around it
	// The sun periodically erupts, forcing the player to hide behind planets or take damage
	//================================================================================================================================
	class SolarEruption : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld& _world, SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		enum State { WAITING = 0, WARMING = 1, COLLAPSING = 2, EXPODING= 3, BACK_TO_NORMAL = 4, SIZE = 5 };

		static void Start( EcsWorld& _world );
		static void Step( EcsWorld& _world, const float _delta );
		static std::string StateToString( const State _state );

		ComponentPtr<Material>			sunlightMaterial;
		ComponentPtr<MeshRenderer>		sunlightRenderer;
		ComponentPtr<ParticleEmitter>	particleEmitter;
		ComponentPtr<PointLight>		sunlightLight;
		
		float previousLightAttenuation;

		float   timer;				// time accumulator
		bool	enabled;
		State	state;
		float	cooldown;			// time between eruption
		float	randomCooldown;		// adds [0-randomCooldown] seconds to the eruption cooldown
		float	damagePerSecond;	// damage taken if the player stays in the sunlight

		float   stateDuration[SIZE];
		float	stateLightAttenuation[SIZE];
		float   stateParticlePerSecond[SIZE];
		float	stateParticleSpeed[SIZE];
		float	stateParticleDuration[SIZE];
		Color	stateParticleColor[SIZE];
	};
}