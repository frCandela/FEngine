#include "fanGlobalIncludes.h"
#include "game/fanHealth.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan
{
	REGISTER_TYPE_INFO( Health, TypeInfo::Flags::EDITOR_COMPONENT )

	//================================================================================================================================
	//================================================================================================================================
	void Health::OnAttach()
	{
		Component::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::OnDetach()
	{
		Component::OnDetach();
	}

	//================================================================================================================================
	// Try to remove energy 
	// If there is not enough energy available, do nothing and return false
	//================================================================================================================================
	bool Health::TryRemoveHealth( const float _healthConsumed )
	{
		assert( _healthConsumed >= 0.f );

		if( m_currentHealth >= _healthConsumed ) {
			m_currentHealth -= _healthConsumed;
			if( m_currentHealth <= 0.f ){ onFallToZero.Emmit(); }
			return true;
		}
		else
		{
			 return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::AddHealth( const float _healthAdded )
	{
		assert( _healthAdded >= 0.f );
		m_currentHealth = std::min( m_currentHealth + _healthAdded, m_maxHealth );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Health::OnGui()
	{
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::SliderFloat( "current health", &m_currentHealth, 0.f, m_maxHealth );
			ImGui::DragFloat( "max health", &m_maxHealth );

		} ImGui::PopItemWidth();
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool Health::Save( Json & _json ) const
	{
		Component::Save( _json );
		SaveFloat( _json, "max_energy", m_maxHealth );

		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool Health::Load( const Json & _json )
	{
		LoadFloat( _json, "max_energy", m_maxHealth );

		return true;
	}
}