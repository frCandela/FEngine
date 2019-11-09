#pragma once

#include "scene/actors/fanActor.h"

namespace fan {

	class Rigidbody;
	class ParticleSystem;
	class WithEnergy;

	//================================================================================================================================
	//================================================================================================================================
	class SpaceShip : public Actor {
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
		float m_lowForwardForce		= 2000.f;
		float m_forwardForce		= 3000.f;
		float m_highForwardForce	= 4000.f;
		float m_lateralForce		= 2000.f;
		float m_drag				= 0.930f;
		float m_energyConsumedPerUnitOfForce = 0.001f;

		Rigidbody * m_rigidbody;
		ParticleSystem * m_particleSystem;
		WithEnergy * m_energy;
	};
}