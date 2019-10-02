#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SpaceShip : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;
		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		void OnGui() override;
		bool IsUnique() const override { return true; }

		DECLARE_EDITOR_COMPONENT(SpaceShip)
		DECLARE_TYPE_INFO(SpaceShip);
	protected:
	private:
		float m_velocity = 8.f;
		float m_rotationSpeed = 3.f;
		float m_drag = 0.99f;

		btVector3 m_speed;
	};
}