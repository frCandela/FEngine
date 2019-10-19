#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SolarSystem : public Actor {
	public:

		void Start() override;
		void Update( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::SOLAR_SYSTEM; }

		DECLARE_EDITOR_COMPONENT( SolarSystem )
		DECLARE_TYPE_INFO( SolarSystem, Actor );
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		int m_seed = 1;

		float m_maxRadius = 30.f;
		float m_initialRadius = 7.2f;
		float m_offset = 1.95f;
		float m_randomScale = 0.62f;

		float m_initialPlanetScale = 1.47f;
		float m_scaleMult = 0.39f;

		float m_minSpeed = 0.15f;
		float m_maxSpeed = 0.36f;
	};
}