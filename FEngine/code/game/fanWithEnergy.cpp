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
	void WithEnergy::Update( const float _delta ){
		AddEnergy( _delta * m_energyGenPerSecond );
	
	}

	void WithEnergy::LateUpdate( const float /*_delta*/ )
	{

		const float ratio = m_currentEnergy / m_maxEnergy;

		ImGui::SetNextWindowSize({300,38});
		ImGui::Begin("", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar ); {
			ImGui::ProgressBar( ratio );
		} ImGui::End();
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::AddEnergy( const float _energy )
	{
		m_currentEnergy = std::clamp( m_currentEnergy + _energy, 0.f, m_maxEnergy );
	}

	//================================================================================================================================
	//================================================================================================================================
	void WithEnergy::OnGui()
	{
		ImGui::DragFloat( "current energy 	   ", &m_currentEnergy );
		ImGui::DragFloat( "max energy 		   ", &m_maxEnergy );
		ImGui::DragFloat( "energy gen / second", &m_energyGenPerSecond );
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool WithEnergy::Save( Json & _json ) const
	{
		Actor::Save( _json );
		SaveFloat( _json, "max_energy 		   ", m_maxEnergy );
		SaveFloat( _json, "energy_gen_per_second", m_energyGenPerSecond );

		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool WithEnergy::Load( Json & _json )
	{
		Actor::Load( _json );
		LoadFloat( _json, "current_energy	   ", m_currentEnergy );
		LoadFloat( _json, "max_energy 		   ", m_maxEnergy );
		LoadFloat( _json, "energy_gen_per_second", m_energyGenPerSecond );

		return true;
	}


}