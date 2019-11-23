#pragma once

#include "scene/actors/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Planet : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void SetSpeed( const float _speed );
		void SetRadius( const float _radius );
		void SetPhase( const float _phase );

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::PLANET; }

		DECLARE_TYPE_INFO(Planet, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:

		ecsPlanet* const m_planet = nullptr;
	};
}