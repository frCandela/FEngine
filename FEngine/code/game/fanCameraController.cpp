#include "fanGlobalIncludes.h"
#include "game/fanCameraController.h"

#include "game/fanPlayersManager.h"
#include "core/time/fanTime.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"

namespace fan {
	REGISTER_TYPE_INFO(CameraController, TypeInfo::Flags::EDITOR_COMPONENT, "game/" )

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::Start() 
	{
		REQUIRE_COMPONENT( Camera, m_camera)
		REQUIRE_TRUE( *m_playersManager != nullptr, "CameraController: missing reference to the PlayersManager" )

		m_gameobject->GetScene()->SetMainCamera(m_camera);
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::LateUpdate( const float /*_delta*/ )  
	{

		const std::vector< Gameobject * > players = m_playersManager->GetPlayers();
		if(  ! players.empty() ) 
		{
			// bounding box
			btVector3 low ( 10000.f, 0.f, 10000.f );
			btVector3 high( -10000.f, 0.f, -10000.f );

			// calculates player center and bounding box
			btVector3 center = btVector3::Zero();
			for ( int playerIndex = 0; playerIndex < players.size(); playerIndex++ )
			{
				btVector3 position =  players[playerIndex]->GetTransform()->GetPosition();
				center += position;

				// bounding box
				low[0] = low[0] < position[0] ? low[0] : position[0];
				low[2] = low[2] < position[2] ? low[2] : position[2];
				high[0] = high[0] > position[0] ? high[0] : position[0];
				high[2] = high[2] > position[2] ? high[2] : position[2];
			}
			center /= (float)players.size();
			assert( low[0] <= high[0] && low[2] <= high[2]);


			m_gameobject->GetTransform()->SetPosition( center + m_heightFromTarget * btVector3::Up() );

			if ( players.size() == 1 )
			{
				m_camera->SetOrthoSize( 10.f );
			}
			else
			{
				const float aspectRatio = m_camera->GetAspectRation();
				const float requiredSizeX = 0.5f * ( 1.f + m_marginRatio[0] ) * ( high[0] - low[0] ) / aspectRatio;
				const float requiredSizeZ = ( 1.f + m_marginRatio[1] ) * ( high[2] - low[2] ) / aspectRatio;

				const float orthoSize =  std::max ( std::max( requiredSizeX, requiredSizeZ ), m_minOrthoSize );
				m_camera->SetOrthoSize(  orthoSize );
			}
		}
	}

	//==================================================z==============================================================================
	//================================================================================================================================
	void CameraController::Update(const float /*_delta*/)
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::OnGui()
	{
		Actor::OnGui();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanComponent("players manager", &m_playersManager );
			ImGui::DragFloat( "height from target", &m_heightFromTarget, 0.25f, 0.5f, 30.f );
			ImGui::DragFloat2( "margin ratio", &m_marginRatio[0], 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "minSize", &m_minOrthoSize, 0.1f, 0.f, 100.f );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool CameraController::Load( const Json & _json ) {
		Actor::Load(_json);

		LoadVec2( _json, "margin_ratio", m_marginRatio);
		LoadFloat( _json, "min_size", m_minOrthoSize);
		LoadFloat( _json, "height_from_target", m_heightFromTarget );
		LoadComponentPtr(_json, "players_manager", m_playersManager );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool CameraController::Save( Json & _json ) const {
		Actor::Save( _json );

		SaveVec2( _json, "margin_ratio", m_marginRatio );
		SaveFloat( _json, "min_size", m_minOrthoSize );
		SaveFloat( _json, "height_from_target", m_heightFromTarget );
		SaveComponentPtr(_json, "players_manager", m_playersManager );	
		
		return true;
	}
}