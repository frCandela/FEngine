#pragma  once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SpaceShip : public Component
	{
		DECLARE_COMPONENT( SpaceShip )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _component );
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

// 		WithEnergy* m_energy;
// 		PlayerInput* m_input;
// 		Health* m_health;
//		Rigidbody* m_rigidbody;
// 		ComponentPtr<ParticleSystem> m_fastForwardParticles;
// 		ComponentPtr<ParticleSystem> m_slowForwardParticles;
// 		ComponentPtr<ParticleSystem> m_reverseParticles;
// 		ComponentPtr<ParticleSystem> m_leftParticles;
// 		ComponentPtr<ParticleSystem> m_rightParticles;

//		void OnContactStarted( Rigidbody* _rb, btPersistentManifold* const& _manifold );
//		void Die();
	};
	static constexpr size_t sizeof_spaceship = sizeof( SpaceShip );
}