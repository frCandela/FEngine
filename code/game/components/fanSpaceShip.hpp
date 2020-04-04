#pragma  once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SpaceShip : public Component
	{
		DECLARE_COMPONENT( SpaceShip )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		enum SpeedMode { REVERSE = 0, SLOW, NORMAL, FAST };

		btVector4 forwardForces;
		float lateralForce;
		float activeDrag;
		float passiveDrag;
		float energyConsumedPerUnitOfForce;
		float remainingChargeEnergy;
		float planetDamage;
		float collisionRepulsionForce;

		ComponentPtr<ParticleEmitter> fastForwardParticlesL;
		ComponentPtr<ParticleEmitter> fastForwardParticlesR;
		ComponentPtr<ParticleEmitter> slowForwardParticlesL;
		ComponentPtr<ParticleEmitter> slowForwardParticlesR;
		ComponentPtr<ParticleEmitter> reverseParticles;
		ComponentPtr<ParticleEmitter> leftParticles;
		ComponentPtr<ParticleEmitter> rightParticles;

		PrefabPtr deathFx;
	};
	static constexpr size_t sizeof_spaceship = sizeof( SpaceShip );
}