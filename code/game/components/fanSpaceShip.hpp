#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "engine/components/fanParticleEmitter.hpp"
#include "engine/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	// vehicle controlled by the player
	//========================================================================================================
	struct SpaceShip : public EcsComponent
	{
		ECS_COMPONENT( SpaceShip )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		enum SpeedMode { REVERSE = 0, SLOW, NORMAL, FAST };

		btVector4 mForwardForces;
		float     mLateralForce;
		float     mActiveDrag;
		float     mPassiveDrag;
		float     mEnergyConsumedPerUnitOfForce;
		float     mPlanetDamage;		// @todo remove this. Planets should have a damage component
		float     mCollisionRepulsionForce;

		ComponentPtr<ParticleEmitter> mFastForwardParticlesL;
		ComponentPtr<ParticleEmitter> mFastForwardParticlesR;
		ComponentPtr<ParticleEmitter> mSlowForwardParticlesL;
		ComponentPtr<ParticleEmitter> mSlowForwardParticlesR;
		ComponentPtr<ParticleEmitter> mReverseParticles;
		ComponentPtr<ParticleEmitter> mLeftParticles;
		ComponentPtr<ParticleEmitter> mRightParticles;

		PrefabPtr mDeathFx;
	};
}