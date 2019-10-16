#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SpaceShip : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;

		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(SpaceShip)
		DECLARE_TYPE_INFO(SpaceShip, Actor );
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_velocity = 8.f;
		float m_rotationSpeed = 3.f;
		float m_drag = 0.99f;

		btVector3 m_speed;
	};
}