#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Satellite : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;
		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		void OnGui() override;
		bool IsUnique() const override { return true; }

		DECLARE_EDITOR_COMPONENT(Satellite)
		DECLARE_TYPE_INFO(Satellite);
	protected:
	private:

		float m_speed = 1.f;
		float m_radius = 1.f;
		float m_phase = 0.f;
	};
}