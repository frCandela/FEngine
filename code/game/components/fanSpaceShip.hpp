#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// vehicle controlled by the player
	//==============================================================================================================================================================
	struct SpaceShip : public EcsComponent
	{
		ECS_COMPONENT( SpaceShip )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		enum SpeedMode { REVERSE = 0, SLOW, NORMAL, FAST };

		btVector4 forwardForces;
		float lateralForce;
		float activeDrag;
		float passiveDrag;
		float energyConsumedPerUnitOfForce;
		float remainingChargeEnergy;
		float planetDamage;		// @todo remove this. Planets should have a damage component
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