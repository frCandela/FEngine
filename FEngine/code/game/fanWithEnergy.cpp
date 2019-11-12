#include "fanGlobalIncludes.h"
#include "game/fanWithEnergy.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT( WithEnergy )
	REGISTER_TYPE_INFO( WithEnergy )

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::Start()
	{}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::OnAttach()
	{
		Actor::OnAttach();

		m_currentEnergy = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::Update( const float /*_delta*/ ){

	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::LateUpdate( const float /*_delta*/ )
	{
		const float ratio = m_currentEnergy / m_maxEnergy;

		ImGui::SetNextWindowSize( { 300,62 } );
		ImGui::Begin( "##player_status_bars", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
		{
			ImGui::PushStyleColor( ImGuiCol_PlotHistogram, Color::Yellow.ToImGui());
			ImGui::ProgressBar( ratio );
			ImGui::PopStyleColor();
		} ImGui::End();
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
		Actor::Save( _json );
		SaveFloat( _json, "max_energy 		   ", m_maxEnergy );

		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool WithEnergy::Load( Json & _json )
	{
		Actor::Load( _json );
		LoadFloat( _json, "max_energy 		   ", m_maxEnergy );

		return true;
	}


}