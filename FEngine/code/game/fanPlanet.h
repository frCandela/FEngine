#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Planet : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;
		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		void OnGui() override;
		bool IsUnique() const override { return true; }

		void SetSpeed( const float _speed )		{ m_speed = _speed; };
		void SetRadius( const float _radius )	{ m_radius = _radius; };
		void SetPhase( const float _phase )		{ m_phase = _phase; };

		DECLARE_EDITOR_COMPONENT(Planet)
		DECLARE_TYPE_INFO(Planet);
	protected:
	private:

		float m_speed = 1.f;
		float m_radius = 1.f;
		float m_phase = 0.f;
	};
}