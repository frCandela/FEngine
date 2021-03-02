#include "fanSpaceShip.hpp"

#include "engine/fanEngineSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &SpaceShip::Load;
		_info.save        = &SpaceShip::Save;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SpaceShip& spaceship					= static_cast<SpaceShip&>( _component );

		spaceship.mForwardForces                = btVector4( 1000.f, 1000.f, 2000.f, 3500.f );
		spaceship.mLateralForce                 = 2000.f;
		spaceship.mActiveDrag                   = 0.930f;
		spaceship.mPassiveDrag                  = 0.950f;
		spaceship.mEnergyConsumedPerUnitOfForce = 0.001f;
		spaceship.mPlanetDamage                 = 5.f;
		spaceship.mCollisionRepulsionForce      = 500.f;

		spaceship.mFastForwardParticlesL.Init( _world );
		spaceship.mFastForwardParticlesR.Init( _world );
		spaceship.mSlowForwardParticlesL.Init( _world );
		spaceship.mSlowForwardParticlesR.Init( _world );
		spaceship.mReverseParticles.Init( _world );
		spaceship.mLeftParticles.Init( _world );
		spaceship.mRightParticles.Init( _world );

		spaceship.mDeathFx.Set( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Save( const EcsComponent& _component, Json& _json )
	{
		const SpaceShip& spaceship = static_cast<const SpaceShip&>( _component );

		Serializable::SaveVec4( _json, "forward_forces", spaceship.mForwardForces );
		Serializable::SaveFloat( _json, "lateral_force", spaceship.mLateralForce );
		Serializable::SaveFloat( _json, "active_drag", spaceship.mActiveDrag );
		Serializable::SaveFloat( _json, "passive_drag", spaceship.mPassiveDrag );
		Serializable::SaveFloat( _json, "energy_consumed_per_unit_of_force", spaceship.mEnergyConsumedPerUnitOfForce );
		Serializable::SaveFloat( _json, "planet_damage", spaceship.mPlanetDamage );
		Serializable::SaveFloat( _json, "collision_repulsion_force", spaceship.mCollisionRepulsionForce );
		Serializable::SaveComponentPtr( _json, "fast_forward_particlesR", spaceship.mFastForwardParticlesR );
		Serializable::SaveComponentPtr( _json, "fast_forward_particlesL", spaceship.mFastForwardParticlesL );
		Serializable::SaveComponentPtr( _json, "slow_forward_particlesR", spaceship.mSlowForwardParticlesR );
		Serializable::SaveComponentPtr( _json, "slow_forward_particlesL", spaceship.mSlowForwardParticlesL );
 		Serializable::SaveComponentPtr( _json, "reverse_particles", spaceship.mReverseParticles );
 		Serializable::SaveComponentPtr( _json, "left_particles", spaceship.mLeftParticles );
 		Serializable::SaveComponentPtr( _json, "right_particles", spaceship.mRightParticles );
		Serializable::SavePrefabPtr( _json, "death_fx", spaceship.mDeathFx );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Load( EcsComponent& _component, const Json& _json )
	{
		SpaceShip& spaceship = static_cast<SpaceShip&>( _component );

		Serializable::LoadVec4( _json, "forward_forces", spaceship.mForwardForces );
		Serializable::LoadFloat( _json, "lateral_force", spaceship.mLateralForce );
		Serializable::LoadFloat( _json, "active_drag", spaceship.mActiveDrag );
		Serializable::LoadFloat( _json, "passive_drag", spaceship.mPassiveDrag );
		Serializable::LoadFloat( _json, "energy_consumed_per_unit_of_force", spaceship.mEnergyConsumedPerUnitOfForce );
		Serializable::LoadFloat( _json, "planet_damage", spaceship.mPlanetDamage );
		Serializable::LoadFloat( _json, "collision_repulsion_force", spaceship.mCollisionRepulsionForce );
		Serializable::LoadComponentPtr( _json, "fast_forward_particlesL", spaceship.mFastForwardParticlesL );
		Serializable::LoadComponentPtr( _json, "fast_forward_particlesR", spaceship.mFastForwardParticlesR );
		Serializable::LoadComponentPtr( _json, "slow_forward_particlesL", spaceship.mSlowForwardParticlesL );
		Serializable::LoadComponentPtr( _json, "slow_forward_particlesR", spaceship.mSlowForwardParticlesR );
		Serializable::LoadComponentPtr( _json, "reverse_particles", spaceship.mReverseParticles );
		Serializable::LoadComponentPtr( _json, "left_particles", spaceship.mLeftParticles );
 		Serializable::LoadComponentPtr( _json, "right_particles", spaceship.mRightParticles );
		Serializable::LoadPrefabPtr( _json, "death_fx", spaceship.mDeathFx );
	}
}