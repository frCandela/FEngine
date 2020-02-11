#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/actors/fanActor.hpp"

namespace fan
{
	class WithEnergy;
	class SolarEruption;

	//================================================================================================================================
	//================================================================================================================================
	class SolarPanel : public Actor
	{
	public:
		void Start() override;
		void Stop() override {}
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;

		float GetChargingRate() const { return m_currentChargingRate; }
		float GetMaxChargingRate() const { return m_maxChargingRate; }
		bool  IsInsideSunlight() const { return m_isInsideSunlight; }

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		DECLARE_TYPE_INFO( SolarPanel, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;

	private:
		bool m_isInsideSunlight = true;
		float m_currentChargingRate = 0.f;	// Energy/s

		float m_minChargingRate = 0.f;		// Energy/s
		float m_maxChargingRate = 10.f;		// Energy/s
		float m_minRange = 2.f;				// Distance at which the solar panel reaches maximum charging rate
		float m_maxRange = 30.f;			// Distance at which the solar panel reaches minimum charging rate

		WithEnergy* m_energy;
		SolarEruption* m_eruption;

		void ComputeChargingRate();
	};
}