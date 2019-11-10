#pragma once

#include "scene/actors/fanActor.h"
#include "scene/fanRessourcePtr.h"

namespace fan {

	class Rigidbody;
	class ParticleSystem;
	class WithEnergy;

	//================================================================================================================================
	//================================================================================================================================
	class SpaceShip : public Actor {
	private:
		enum SpeedMode{ REVERSE = 0, SLOW, NORMAL, FAST };

	public:
		void Start() override;
		void Update(const float _delta) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::SPACE_SHIP; }

		DECLARE_EDITOR_COMPONENT(SpaceShip)
		DECLARE_TYPE_INFO(SpaceShip, Actor );
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:

		btVector4 m_forwardForces = btVector4(1000.f,1000.f,2000.f,3500.f);
		float m_lateralForce		= 2000.f;
		float m_activeDrag			= 0.930f;
		float m_passiveDrag			= 0.950f;
		float m_energyConsumedPerUnitOfForce = 0.001f;

		float m_remainingChargeEnergy = 0.f;

		// References
		Rigidbody * m_rigidbody;
		WithEnergy * m_energy;

		GameobjectPtr m_fastForwardParticlesGo;
		GameobjectPtr m_slowForwardParticlesGo;
		GameobjectPtr m_reverseParticlesGo;
		GameobjectPtr m_leftParticlesGo;
		GameobjectPtr m_rightParticlesGo;
		 
		ParticleSystem * m_fastForwardParticles = nullptr;
		ParticleSystem * m_slowForwardParticles = nullptr;
		ParticleSystem * m_reverseParticles = nullptr;
		ParticleSystem * m_leftParticles = nullptr;
		ParticleSystem * m_rightParticles = nullptr;

	};
}