#pragma once

#include "scene/actors/fanActor.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	class SunLight : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;
		void LateUpdate( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_TYPE_INFO( SunLight, Actor );
	protected:

		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
	};
}