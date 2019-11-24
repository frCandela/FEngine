#include "fanGlobalIncludes.h"
#include "game/fanPlayersManager.h"

#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "core/input/fanJoystick.h"

namespace fan
{
	REGISTER_TYPE_INFO( PlayersManager, TypeInfo::Flags::EDITOR_COMPONENT )

		//================================================================================================================================
		//================================================================================================================================
		void PlayersManager::Start()
	{
		REQUIRE_TRUE( *m_playerPrefab != nullptr, "PlayersManager : missing player prefab " )
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnAttach()
	{
		Actor::OnAttach();
		m_gameobject->GetScene()->onScenePlay.Connect( &PlayersManager::OnScenePlay, this );
		m_gameobject->GetScene()->onScenePause.Connect( &PlayersManager::OnScenePause, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnDetach()
	{
		Actor::OnDetach();
		m_gameobject->GetScene()->onScenePlay.Disconnect( &PlayersManager::OnScenePlay, this );
		m_gameobject->GetScene()->onScenePause.Disconnect( &PlayersManager::OnScenePause, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnScenePlay()
	{
		Joystick::Get().onJoystickConnect.Connect( &PlayersManager::OnJoystickConnect, this );

		// Spawn mouse player
		AddPlayer( s_mousePlayerID,  "mouse_player" );

		// Spawn joystick players
		for ( int joystickIndex = 0; joystickIndex <= GLFW_JOYSTICK_LAST; joystickIndex++ )
		{
			if ( Joystick::Get().IsConnected( joystickIndex ) )
			{
				std::stringstream ss;
				ss << "joystick" << joystickIndex << "_player";
				AddPlayer( joystickIndex, ss.str() );
			}
		}
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnScenePause()
	{
		Joystick::Get().onJoystickConnect.Disconnect( &PlayersManager::OnJoystickConnect, this );

		// Remove all players
		while ( ! m_players.empty() )
		{			
			RemovePlayer( m_players.begin()->first );
		}
	}

	//================================================================================================================================
	// Instanciates a player in the scene
	//================================================================================================================================
	void PlayersManager::AddPlayer( const int _ID, const std::string& _name )
	{
		if ( *m_playerPrefab != nullptr )
		{
			assert( m_players.find( _ID ) ==  m_players.end() );
			Gameobject * mousePlayer = m_gameobject->GetScene()->CreateGameobject( **m_playerPrefab, m_gameobject );
			mousePlayer->SetEditorFlags( mousePlayer->GetEditorFlags() | Gameobject::EditorFlag::NOT_SAVED );
			mousePlayer->SetName( _name );
			m_players[_ID] = mousePlayer;
		}
	}

	//================================================================================================================================
	// Removes a player from the scene
	//================================================================================================================================
	void PlayersManager::RemovePlayer( const int _ID )
	{
		assert( m_players.find( _ID ) != m_players.end() );

		Gameobject * player = m_players[_ID];
		m_players.erase( _ID );
		m_gameobject->GetScene()->DeleteGameobject( player );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnJoystickConnect( int _joystickID, bool _connected )
	{
		if ( _connected )
		{
			std::stringstream ss;
			ss << "joystick" << _joystickID << "_player";
			AddPlayer(_joystickID, ss.str() );
		}
		else
		{			
			RemovePlayer( _joystickID );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::Update( const float /*_delta*/ )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnGui()
	{
		Actor::OnGui();
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanPrefab( "player prefab", &m_playerPrefab );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayersManager::Load( const Json & _json )
	{
		Actor::Load( _json );
		LoadPrefabPtr( _json, "player_prefab", m_playerPrefab );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayersManager::Save( Json & _json ) const
	{
		Actor::Save( _json );
		SavePrefabPtr( _json, "player_prefab", m_playerPrefab );
		return true;
	}
}