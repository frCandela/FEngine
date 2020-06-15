#pragma once

#include "ecs/fanEcsSingleton.hpp"

#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanPointLight.hpp"

namespace fan
{
	struct Game;

	//================================================================================================================================
	// SolarEruption is the fx of the sun exploding out and projecting matter all around it
	// The sun periodically erupts, forcing the player to hide behind planets or take damage
	//================================================================================================================================
	class SolarEruption : public EcsSingleton
	{
		ECS_SINGLETON( SolarEruption )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld& _world, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );
		static void NetSave( const EcsSingleton& _component, sf::Packet& _packet );
		static void NetLoad( EcsSingleton& _component, sf::Packet& _packet );

		enum State { WAITING = 0, WARMING = 1, COLLAPSING = 2, EXPODING= 3, BACK_TO_NORMAL = 4, SIZE = 5 };

		static void  Start( EcsWorld& _world );
		static void  Step( EcsWorld& _world, const float _delta );

		ComponentPtr<Material>			sunlightMaterial;
		ComponentPtr<MeshRenderer>		sunlightRenderer;
		ComponentPtr<ParticleEmitter>	particleEmitter;
		ComponentPtr<PointLight>		sunlightLight;
		
		float previousLightAttenuation;

		FrameIndex eruptionStartFrame;// the eruption starts at this frame @todo make a spawner for this

		bool	enabled;
		State	state;
		float   timer;				// time accumulator
		float	cooldown;			// time between eruption in seconds
		float	randomCooldown;		// adds [0-randomCooldown] seconds to the eruption cooldown
		float	damagePerSecond;	// damage taken if the player stays in the sunlight

		float   stateDuration[SIZE];
		float	stateLightAttenuation[SIZE];
		float   stateParticlePerSecond[SIZE];
		float	stateParticleSpeed[SIZE];
		float	stateParticleDuration[SIZE];
		Color	stateParticleColor[SIZE];

		static FrameIndex CalculateNextEruptionStartFrame( const SolarEruption& _eruption, const Game& game );
	};
}