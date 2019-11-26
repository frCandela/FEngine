#include "fanGlobalIncludes.h"
#include "game/fanSolarPanel.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "game/fanSolarEruption.h"
#include "game/fanWithEnergy.h"
#include "renderer/fanMesh.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	REGISTER_TYPE_INFO( SolarPanel, TypeInfo::Flags::EDITOR_VISIBLE )

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::Start()
	{
		REQUIRE_COMPONENT( WithEnergy, m_energy );
		REQUIRE_SCENE_COMPONENT( SolarEruption, m_eruption );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::OnAttach()
	{
		Actor::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::Update( const float _delta ) {
		ComputeChargingRate();		
		m_energy->AddEnergy( _delta * m_currentChargingRate );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::LateUpdate( const float /*_delta*/ )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::OnGui()
	{
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "min_charging_rate", &m_minChargingRate, 0.5f, 0.f, 100.f );
			ImGui::DragFloat( "max_charging_rate", &m_maxChargingRate, 0.5f, 0.f, 100.f );
			ImGui::DragFloat( "low_range		", &m_minRange, 0.5f, 0.f, 100.f );
			ImGui::DragFloat( "max_range 		", &m_maxRange, 0.5f, 0.f, 100.f );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::ComputeChargingRate()
	{
		// Sunlight mesh raycast
		const btVector3 rayOrigin = m_gameobject->GetTransform()->GetPosition() + btVector3::Up();
		btVector3 outIntersection;
		m_isInsideSunlight = ecsSolarEruptionMeshSystem::s_mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );

		// Charging rate
		if( m_isInsideSunlight ) {
			const btVector3 position = m_gameobject->GetTransform()->GetPosition();
			const float distance = position.norm();
			const float slope = ( m_maxChargingRate - m_minChargingRate ) / ( m_maxRange - m_minRange );
			const float unclampedRate = m_maxChargingRate - slope * ( distance - m_minRange );
			m_currentChargingRate = std::clamp( unclampedRate, m_minChargingRate, m_maxChargingRate );
		}
		else
		{
			m_currentChargingRate = 0.f;
		}
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool SolarPanel::Save( Json & _json ) const
	{
		Actor::Save( _json );
		
		SaveFloat( _json, "min_charging_rate", m_minChargingRate );
		SaveFloat( _json, "max_charging_rate", m_maxChargingRate );
		SaveFloat( _json, "low_range		", m_minRange );
		SaveFloat( _json, "max_range 		", m_maxRange );

		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool SolarPanel::Load( const Json & _json )
	{
		Actor::Load( _json );

		LoadFloat( _json, "min_charging_rate", m_minChargingRate );
		LoadFloat( _json, "max_charging_rate", m_maxChargingRate );
		LoadFloat( _json, "low_range		", m_minRange );
		LoadFloat( _json, "max_range 		", m_maxRange );
		return true;
	}


}