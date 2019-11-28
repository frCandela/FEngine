#include "fanGlobalIncludes.h"
#include "game/fanPlayersManager.h"

#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "core/input/fanJoystick.h"
#include "game/fanPlayerInput.h"
#include "game/fanSpaceShip.h"

namespace fan
{
	REGISTER_TYPE_INFO( PlayersManager, TypeInfo::Flags::EDITOR_COMPONENT, "game/managers/" )

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

		SpaceShip::s_onPlayerDie.Connect( &PlayersManager::OnPlayerDie, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnDetach()
	{
		Actor::OnDetach();
		m_gameobject->GetScene()->onScenePlay.Disconnect( &PlayersManager::OnScenePlay, this );
		m_gameobject->GetScene()->onScenePause.Disconnect( &PlayersManager::OnScenePause, this );

		SpaceShip::s_onPlayerDie.Disconnect( &PlayersManager::OnPlayerDie, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnScenePlay()
	{
		Joystick::Get().onJoystickConnect.Connect( &PlayersManager::OnJoystickConnect, this );

		// Spawn mouse player
		AddPlayer( s_mousePlayerID,  "mouse_player" );

		// Spawn joystick players
		for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
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
			Gameobject * player = m_gameobject->GetScene()->CreateGameobject( **m_playerPrefab, m_gameobject );
			m_players[_ID] = player;

			player->SetEditorFlags( player->GetEditorFlags() | Gameobject::EditorFlag::NOT_SAVED );
			player->SetName( _name );

			PlayerInput * playerInput = player->GetComponent<PlayerInput>();
			if ( playerInput != nullptr )
			{
				playerInput->SetJoystickID(_ID);
				playerInput->SetInputType( _ID < 0 ? PlayerInput::KEYBOARD_MOUSE : PlayerInput::JOYSTICK );
			}
			else
			{
				Debug::Warning("PlayersManager::AddPlayer : Prefab is missing a PlayerInput component.");
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnPlayerDie( Gameobject * _gameobject )
	{
		// Removes dead player from the game
		PlayerInput * input = _gameobject->GetComponent<PlayerInput>();
		if ( input != nullptr )
		{
			RemovePlayer(input->GetJoystickID());
			m_gameobject->GetScene()->DeleteGameobject( _gameobject );
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
	// Returns an array of connected players
	//================================================================================================================================
	std::vector< Gameobject * > PlayersManager::GetPlayers() const
	{
		std::vector< Gameobject * > players;
		players.reserve( m_players.size() );
		for ( auto& pair : m_players )
		{
			players.push_back( pair.second );
		}
		return players;
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