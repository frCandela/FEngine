#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanSolarEruption.hpp"
#include "game/components/fanWithEnergy.hpp"
#include "render/fanMesh.hpp"

namespace fan
{

		//================================================================================================================================
		//================================================================================================================================
		void SolarPanel::Start()
	{
// 		REQUIRE_COMPONENT( WithEnergy, m_energy );
// 		REQUIRE_SCENE_COMPONENT( SolarEruption, m_eruption );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::OnAttach()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::OnDetach()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::Update( const float _delta )
	{
		ComputeChargingRate();
		m_energy->AddEnergy( _delta * m_currentChargingRate );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::LateUpdate( const float /*_delta*/ )
	{}

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
// 		// Sunlight mesh raycast
// 		const btVector3 rayOrigin = m_gameobject->GetTransform().GetPosition() + btVector3::Up();
// 		btVector3 outIntersection;
// 		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();
// 		m_isInsideSunlight = sunLight.mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );
// 
// 		// Charging rate
// 		if ( m_isInsideSunlight )
// 		{
// 			const btVector3 position = m_gameobject->GetTransform().GetPosition();
// 			const float distance = position.norm();
// 			const float slope = ( m_maxChargingRate - m_minChargingRate ) / ( m_maxRange - m_minRange );
// 			const float unclampedRate = m_maxChargingRate - slope * ( distance - m_minRange );
// 			m_currentChargingRate = std::clamp( unclampedRate, m_minChargingRate, m_maxChargingRate );
// 		}
// 		else
// 		{
// 			m_currentChargingRate = 0.f;
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarPanel::Save( Json& _json ) const
	{


// 		Serializable::SaveFloat( _json, "min_charging_rate", m_minChargingRate );
// 		Serializable::SaveFloat( _json, "max_charging_rate", m_maxChargingRate );
// 		Serializable::SaveFloat( _json, "low_range		", m_minRange );
// 		Serializable::SaveFloat( _json, "max_range 		", m_maxRange );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SolarPanel::Load( const Json& _json )
	{


// 		Serializable::LoadFloat( _json, "min_charging_rate", m_minChargingRate );
// 		Serializable::LoadFloat( _json, "max_charging_rate", m_maxChargingRate );
// 		Serializable::LoadFloat( _json, "low_range		", m_minRange );
// 		Serializable::LoadFloat( _json, "max_range 		", m_maxRange );
		return true;
	}


}