#pragma once

#include "scene/components/fanActor.h"

namespace fan {

	class Rigidbody;

	//================================================================================================================================
	//================================================================================================================================
	class SpaceShip : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::SPACE_SHIP; }

		DECLARE_EDITOR_COMPONENT(SpaceShip)
		DECLARE_TYPE_INFO(SpaceShip, Actor );
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_forwardBoost = 3500.f;
		float m_lateralBoost = 2000.f;
		float m_drag = 0.930f;

		Rigidbody * m_rigidbody;
	};
}