#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/actors/fanActor.hpp"
#include "scene/fanComponentPtr.hpp"

namespace fan
{

	class Rigidbody;
	class ParticleSystem;
	class WithEnergy;
	class PlayerInput;
	class Health;

	//================================================================================================================================
	//================================================================================================================================
	class SpaceShip : public Actor
	{
	private:
		enum SpeedMode { REVERSE = 0, SLOW, NORMAL, FAST };

	public:
		Signal<Gameobject*> onPlayerDie;

		void Start() override;
		void Stop() override {}
		void Update( const float _delta ) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::SPACE_SHIP16; }

		DECLARE_TYPE_INFO( SpaceShip, Actor );
	protected:
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;

	private:
		btVector4 m_forwardForces = btVector4( 1000.f, 1000.f, 2000.f, 3500.f );
		float m_lateralForce = 2000.f;
		float m_activeDrag = 0.930f;
		float m_passiveDrag = 0.950f;
		float m_energyConsumedPerUnitOfForce = 0.001f;
		float m_remainingChargeEnergy = 0.f;
		float m_planetDamage = 5.f;
		float m_collisionRepulsionForce = 500.f;

		// References
		Rigidbody* m_rigidbody;
		WithEnergy* m_energy;
		PlayerInput* m_input;
		Health* m_health;

		ComponentPtr<ParticleSystem> m_fastForwardParticles;
		ComponentPtr<ParticleSystem> m_slowForwardParticles;
		ComponentPtr<ParticleSystem> m_reverseParticles;
		ComponentPtr<ParticleSystem> m_leftParticles;
		ComponentPtr<ParticleSystem> m_rightParticles;

		void OnContactStarted( Rigidbody* _rb, btPersistentManifold* const& _manifold );
		void Die();
	};
}