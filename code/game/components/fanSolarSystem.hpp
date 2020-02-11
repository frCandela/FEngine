#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/actors/fanActor.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class SolarSystem : public Actor
	{
	public:

		void Start() override;
		void Stop() override {}
		void Update( const float _delta ) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::SOLAR_SYSTEM16; }

		DECLARE_TYPE_INFO( SolarSystem, Actor );
	protected:
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;

	private:
		int m_seed = 1;

		float m_maxRadius = 30.f;
		float m_initialRadius = 7.2f;
		float m_scaleMult = 0.8f;


		btVector2 m_radiusFactors = btVector2( 0.44f, 0.29f );
		btVector2 m_radiusRFactors = btVector2( 1.f, 1.f );
		btVector2 m_scaleMinMax = btVector2( 0.3f, 0.1f );
		btVector2 m_speedFactors = btVector2( 0.1f, 0.012f );
		int m_maxPlanetsPerOrbit = 3;
	};
}