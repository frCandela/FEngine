#include "fanGlobalIncludes.h"
#include "game/ui/fanSpaceShipUI.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/ui/fanProgressBar.h"
#include "game/fanWithEnergy.h"
#include "game/fanSolarPanel.h"
#include "scene/components/ui/fanUIMeshRenderer.h"
#include "scene/components/fanCamera.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	REGISTER_TYPE_INFO( SpaceShipUI, TypeInfo::Flags::EDITOR_COMPONENT )


	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::OnAttach()
	{
		Actor::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::OnDetach()		
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::Start()
	{
		REQUIRE_TRUE( *m_spaceShip != nullptr,		"SpaceshipUI: missing reference" );
		REQUIRE_TRUE( *m_healthProgress != nullptr,	"SpaceshipUI: missing reference" );
		REQUIRE_TRUE( *m_energyProgress != nullptr, "SpaceshipUI: missing reference" );
		REQUIRE_TRUE( *m_signalProgress != nullptr, "SpaceshipUI: missing reference" );
		REQUIRE_TRUE( *m_signalRenderer != nullptr, "SpaceshipUI: missing reference" );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::Update( const float /*_delta*/ )
	{
		// Update energy progress bar
 		WithEnergy * energy = m_spaceShip->GetComponent<WithEnergy>(); 
		if ( energy != nullptr )
		{
			const float ratio = energy->GetEnergy() / energy->GetMaxEnergy();
			m_energyProgress->SetProgress(ratio);
		}

		// Update signal progress bar
		SolarPanel * solarPanel = m_spaceShip->GetComponent<SolarPanel>();
		if ( solarPanel != nullptr )
		{
			const float ratio = solarPanel->GetChargingRate() / solarPanel->GetMaxChargingRate();
			m_signalProgress->SetProgress( ratio );
			m_signalRenderer->SetColor( GetSignalColor(ratio) );
		}


	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::LateUpdate( const float /*_delta*/ )  
	{
		// Set ui position
		Camera * camera = m_gameobject->GetScene()->GetMainCamera();
		btVector2 screenPos = camera->WorldPosToScreen( m_spaceShip->GetTransform()->GetPosition() );
 		m_gameobject->GetTransform()->SetPosition( btVector3( screenPos[0], screenPos[1], 0.f ) );


		
	}

	//================================================================================================================================
	//================================================================================================================================
	Color SpaceShipUI::GetSignalColor( const float _ratio )
	{
		if ( _ratio < 0.5f )
		{
			return Color( 1.f, 2.f * _ratio, 0.f, 1.f ); // red to orange
		}
		else
		{			
			float ratio = 2.f * (_ratio - 0.5f);
			return Color( 1.f - ratio , 1.f, 0.f, 1.f ); // orange to green
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShipUI::OnGui()
	{
		ImGui::FanGameobject("spaceShip",	   &m_spaceShip);
		ImGui::FanComponent("health progress", &m_healthProgress);
		ImGui::FanComponent("energy progress", &m_energyProgress);
		ImGui::FanComponent("signal progress", &m_signalProgress);
		ImGui::FanComponent("signal renderer", &m_signalRenderer);

		if ( *m_spaceShip != nullptr )
		{
			Camera * camera = m_gameobject->GetScene()->GetMainCamera();
			Ray toto = camera->ScreenPosToRay(m_gameobject->GetTransform()->GetPosition());


			Debug::Render().DebugLine(toto.origin, m_spaceShip->GetTransform()->GetPosition(), Color::Red );
		}
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShipUI::Save( Json & _json ) const
	{
		Actor::Save( _json );
		SaveGameobjectPtr(_json, "spaceship",		m_spaceShip );
		SaveComponentPtr(_json, "health_progress",  m_healthProgress );
		SaveComponentPtr(_json, "energy_progress",  m_energyProgress );
		SaveComponentPtr(_json, "signal_progress",  m_signalProgress);
		SaveComponentPtr( _json, "signal_renderer", m_signalRenderer );

		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool SpaceShipUI::Load( const Json & _json )
	{
		Actor::Load( _json );
		LoadGameobjectPtr(_json, "spaceship",		m_spaceShip );
		LoadComponentPtr( _json, "health_progress", m_healthProgress );
		LoadComponentPtr( _json, "energy_progress", m_energyProgress );
		LoadComponentPtr( _json, "signal_progress",  m_signalProgress);
		LoadComponentPtr( _json, "signal_renderer", m_signalRenderer );
		return true;
	}
}