#include "fanGlobalIncludes.h"
#include "game/ui/fanSpaceShipUI.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/ui/fanProgressBar.h"
#include "game/fanSpaceShip.h"

namespace fan
{
	REGISTER_TYPE_INFO( SpaceShipUI, TypeInfo::Flags::EDITOR_COMPONENT )


	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::OnAttach()
	{
		Component::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::OnDetach()		
	{
		Component::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::Start()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::Update( const float /*_delta*/ )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::OnGui()
	{
		ImGui::FanComponent("spaceShip", &m_spaceShip);
		ImGui::FanComponent("health progress", &m_healthProgress);
		ImGui::FanComponent("energy progress", &m_energyProgress);

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{

		} ImGui::PopItemWidth();

	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShipUI::Save( Json & _json ) const
	{
		Component::Save( _json );
		SaveComponentPtr(_json, "spaceship", m_spaceShip );
		SaveComponentPtr(_json, "health_progress", m_healthProgress );
		SaveComponentPtr(_json, "energy_progress", m_energyProgress );
		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShipUI::Load( Json & _json )
	{
		LoadComponentPtr(_json, "spaceship", m_spaceShip );
		LoadComponentPtr( _json, "health_progress", m_healthProgress );
		LoadComponentPtr( _json, "energy_progress", m_energyProgress );
		return true;
	}
}