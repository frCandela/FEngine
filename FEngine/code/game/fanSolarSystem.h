#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SolarSystem : public Actor {
	public:

		void Start() override;
		void Update( const float _delta ) override;
		bool Load( std::istream& _in ) override;
		bool Save( std::ostream& _out, const int _indentLevel ) const override;

		void OnGui() override;
		bool IsUnique() const override { return true; }

		DECLARE_EDITOR_COMPONENT( SolarSystem )
		DECLARE_TYPE_INFO( SolarSystem );
	protected:
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