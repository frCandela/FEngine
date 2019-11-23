#include "fanGlobalIncludes.h"
#include "game/fanWithEnergy.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan
{
	REGISTER_TYPE_INFO( WithEnergy, TypeInfo::Flags::EDITOR_COMPONENT )

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::OnAttach()
	{
		Component::OnAttach();

		m_currentEnergy = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::OnDetach()
	{
		Component::OnDetach();
	}

	//================================================================================================================================
	// Try to remove energy 
	// If there is not enough energy available, do nothing and return false
	//================================================================================================================================
	bool WithEnergy::TryRemoveEnergy( const float _energyConsumed )
	{
		assert( _energyConsumed >= 0.f );

		if( m_currentEnergy >= _energyConsumed ) {
			m_currentEnergy -= _energyConsumed;
			return true;
		}
		else
		{
			 return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::AddEnergy( const float _energyAdded )
	{
		assert( _energyAdded >= 0.f );
		m_currentEnergy = std::min( m_currentEnergy + _energyAdded, m_maxEnergy );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::OnGui()
	{
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "current energy 	   ", &m_currentEnergy );
			ImGui::DragFloat( "max energy 		   ", &m_maxEnergy );

		} ImGui::PopItemWidth();
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool WithEnergy::Save( Json & _json ) const
	{
		Component::Save( _json );
		SaveFloat( _json, "max_energy 		   ", m_maxEnergy );

		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool WithEnergy::Load( const Json & _json )
	{
		LoadFloat( _json, "max_energy 		   ", m_maxEnergy );

		return true;
	}


}