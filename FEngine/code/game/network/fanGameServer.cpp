#include "fanGlobalIncludes.h"
#include "game/network/fanGameServer.h"

#include "core/input/fanInputManager.h"
#include "core/input/fanInput.h"
#include "core/input/fanMouse.h"
#include "core/input/fanJoystick.h"
#include "network/packets/fanIPacket.h"
#include "game/fanGameManager.h"
#include "game/fanPlayersManager.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"


namespace fan
{
	REGISTER_TYPE_INFO( GameServer, TypeInfo::Flags::EDITOR_COMPONENT, "game/net/" )

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::OnAttach()
	{
		Actor::OnAttach();
		m_server.Create( 53000 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::OnDetach()
	{
		Actor::OnDetach();
		m_server.UnBind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Start()
	{
		if ( !GetScene().IsServer() )
		{
			GetScene().DeleteComponent( this );
			return;
		}

		REQUIRE_COMPONENT( PlayersManager, m_playersManager );
		REQUIRE_COMPONENT( GameManager, m_gameManager );

		if ( m_playersManager )
		{
			m_server.onClientConnected.Connect(&PlayersManager::AddPlayer, m_playersManager);
		}

		m_server.Bind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Stop() 
	{
		m_server.ClearClients();
		m_server.onClientConnected.Disconnect(&PlayersManager::AddPlayer, m_playersManager);
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Update( const float _delta )
	{
		m_server.Update(_delta);
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::StartGame()
	{
		m_server.BroadcastPacket( PacketStartGame().ToPacket() );

		m_playersManager->SpawnSpaceShips();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::LateUpdate( const float /*_delta*/ ){}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::OnGui()
	{
		Actor::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			if ( ImGui::Button( "start game " ) )
			{
				StartGame();
			}
		} 
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameServer::Load( const Json & _json )
	{
		Actor::Load( _json );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameServer::Save( Json & _json ) const
	{
		Actor::Save( _json );
		return true;
	}
}