#pragma once

#include "core/ecs/fanEcsSingleton.hpp"

#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanPointLight.hpp"

namespace fan
{
	struct Time;

	//========================================================================================================
	// SolarEruption is the fx of the sun exploding out and projecting matter all around it
	// The sun periodically erupts, forcing the player to hide behind planets or take damage
	//========================================================================================================
	class SolarEruption : public EcsSingleton
	{
		ECS_SINGLETON( SolarEruption )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		enum State { WAITING = 0, WARMING = 1, COLLAPSING = 2, EXPODING= 3, BACK_TO_NORMAL = 4, SIZE = 5 };

		static std::string StateToString( const SolarEruption::State  _state );
		static void Start( EcsWorld& _world );
		static void Step( EcsWorld& _world, const float _delta );
		void		SpawnEruptionNow();
		void		ScheduleNextEruption( EcsWorld& _world );

		ComponentPtr<Material>        mSunlightMaterial;
		ComponentPtr<MeshRenderer>    mSunlightRenderer;
		ComponentPtr<ParticleEmitter> mParticleEmitter;
		ComponentPtr<PointLight>      mSunlightLight;
		
		float mPreviousLightAttenuation;

		FrameIndex mSpawnFrame; // the frame index at which the eruption is spawn
		bool       mEnabled;
		State      mState;
		float      mTimer;				// time accumulator
		float      mCooldown;			// time between eruption in seconds
		float      mRandomCooldown;		// adds [0-randomCooldown] seconds to the eruption cooldown
		float      mDamagePerSecond;	// damage taken if the player stays in the sunlight

		float mStateDuration[SIZE];
		float mStateLightAttenuation[SIZE];
		float mStateParticlePerSecond[SIZE];
		float mStateParticleSpeed[SIZE];
		float mStateParticleDuration[SIZE];
		Color mStateParticleColor[SIZE];

	};
}