#include "game/components/fanCameraController.hpp"

#include "game/components/fanPlayersManager.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "core/time/fanTime.hpp"

namespace fan
{
	REGISTER_TYPE_INFO( CameraController, TypeInfo::Flags::EDITOR_COMPONENT, "game/" )

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::Start()
	{
		REQUIRE_TRUE( m_camera.IsValid(), "CameraController missing main camera reference" );
		REQUIRE_TRUE( *m_playersManager != nullptr, "CameraController: missing reference to the PlayersManager" )

		m_gameobject->GetScene().SetMainCamera( *m_camera );
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::LateUpdate( const float /*_delta*/ )
	{

		const std::vector< Gameobject* > players = m_playersManager->GetPlayers();
		if ( !players.empty() )
		{
			// bounding box
			btVector3 low( 10000.f, 0.f, 10000.f );
			btVector3 high( -10000.f, 0.f, -10000.f );

			// calculates player center and bounding box
			btVector3 center = btVector3::Zero();
			for ( int playerIndex = 0; playerIndex < players.size(); playerIndex++ )
			{
				btVector3 position = players[ playerIndex ]->GetTransform().GetPosition();
				center += position;

				// bounding box
				low[ 0 ] = low[ 0 ] < position[ 0 ] ? low[ 0 ] : position[ 0 ];
				low[ 2 ] = low[ 2 ] < position[ 2 ] ? low[ 2 ] : position[ 2 ];
				high[ 0 ] = high[ 0 ] > position[ 0 ] ? high[ 0 ] : position[ 0 ];
				high[ 2 ] = high[ 2 ] > position[ 2 ] ? high[ 2 ] : position[ 2 ];
			}
			center /= ( float ) players.size();
			assert( low[ 0 ] <= high[ 0 ] && low[ 2 ] <= high[ 2 ] );


			m_gameobject->GetTransform().SetPosition( center + m_heightFromTarget * btVector3::Up() );

			if ( players.size() == 1 )
			{
				m_camera->SetOrthoSize( 10.f );
			}
			else
			{
				const float aspectRatio = m_camera->GetAspectRatio();
				const float requiredSizeX = 0.5f * ( 1.f + m_marginRatio[ 0 ] ) * ( high[ 0 ] - low[ 0 ] ) / aspectRatio;
				const float requiredSizeZ = ( 1.f + m_marginRatio[ 1 ] ) * ( high[ 2 ] - low[ 2 ] ) / aspectRatio;

				const float orthoSize = std::max( std::max( requiredSizeX, requiredSizeZ ), m_minOrthoSize );
				m_camera->SetOrthoSize( orthoSize );
			}
		}
	}

	//==================================================z==============================================================================
	//================================================================================================================================
	void CameraController::Update( const float /*_delta*/ )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::OnGui()
	{
		Actor::OnGui();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanComponent( "players manager", m_playersManager );
			ImGui::FanComponent( "camera", m_camera );
			ImGui::DragFloat( "height from target", &m_heightFromTarget, 0.25f, 0.5f, 30.f );
			ImGui::DragFloat2( "margin ratio", &m_marginRatio[0], 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "minSize", &m_minOrthoSize, 0.1f, 0.f, 100.f );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void CameraController::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool CameraController::Load( const Json& _json )
	{
		Actor::Load( _json );

		Serializable::LoadVec2( _json, "margin_ratio", m_marginRatio );
		Serializable::LoadFloat( _json, "min_size", m_minOrthoSize );
		Serializable::LoadFloat( _json, "height_from_target", m_heightFromTarget );
		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "players_manager", m_playersManager );
		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "camera", m_camera );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool CameraController::Save( Json& _json ) const
	{
		Actor::Save( _json );

		Serializable::SaveVec2( _json, "margin_ratio", m_marginRatio );
		Serializable::SaveFloat( _json, "min_size", m_minOrthoSize );
		Serializable::SaveFloat( _json, "height_from_target", m_heightFromTarget );
		Serializable::SaveComponentPtr( _json, "players_manager", m_playersManager );
		Serializable::SaveComponentPtr( _json,"camera", m_camera );

		return true;
	}
}