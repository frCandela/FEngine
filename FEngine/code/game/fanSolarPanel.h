#pragma once

#include "scene/actors/fanActor.h"

namespace fan
{
	class WithEnergy;

	//================================================================================================================================
	//================================================================================================================================
	class SolarPanel : public Actor
	{
	public:
		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_EDITOR_COMPONENT( SolarPanel )
		DECLARE_TYPE_INFO( SolarPanel, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_currentChargingRate = 0.f;	// Energy/s
		float m_minChargingRate = 0.f;		// Energy/s
		float m_maxChargingRate = 10.f;		// Energy/s
		float m_minRange = 2.f;				// Distance at which the solar panel reaches maximum charging rate
		float m_maxRange = 30.f;			// Distance at which the solar panel reaches minimum charging rate

		WithEnergy * m_energy;

		void ComputeChargingRate();
	};
}