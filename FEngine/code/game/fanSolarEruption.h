#pragma once

#include "scene/actors/fanActor.h"

namespace fan {
	class Mesh;

	//================================================================================================================================
	//================================================================================================================================
	class SolarEruption : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_EDITOR_COMPONENT( SolarEruption )
		DECLARE_TYPE_INFO( SolarEruption, Actor );
	protected:

		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		Mesh* m_mesh;

		float m_radius = 10.f;
		float m_subAngle = 45.f;
		bool m_debugDraw = false;

	};
}