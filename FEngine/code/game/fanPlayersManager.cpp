#include "fanGlobalIncludes.h"
#include "game/fanPlayersManager.h"

#include "core/input/fanJoystick.h"
#include "game/fanPlayerInput.h"
#include "game/fanSpaceShip.h"
#include "game/network/fanPlayerNetwork.h"

namespace fan
{
	REGISTER_TYPE_INFO( PlayersManager, TypeInfo::Flags::EDITOR_COMPONENT, "game/managers/" )

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::Start()
	{
		REQUIRE_TRUE( *m_playerPrefab != nullptr, "PlayersManager : missing player prefab " )

		Joystick::Get().onJoystickConnect.Connect( &PlayersManager::OnJoystickConnect, this );


		// Spawn mouse player
		AddPlayer( s_mousePlayerID, "mouse_player" );

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
	void PlayersManager::Stop()
	{
		Joystick::Get().onJoystickConnect.Disconnect( &PlayersManager::OnJoystickConnect, this );

		// Remove all players
		while ( !m_players.empty() )
		{
			RemovePlayer( m_players.begin()->first );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnAttach()
	{
		Actor::OnAttach();

	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	// Instanciates a player in the scene
	//================================================================================================================================
	void PlayersManager::AddPlayer( const int _ID, const std::string& _name )
	{
		if ( *m_playerPrefab != nullptr )
		{
			// Creates a persistent gameobject for the player
			assert( m_players.find( _ID ) ==  m_players.end() );		
			PlayerData playerdata;
			playerdata.persistent = GetScene().CreateGameobject(_name + std::string("_persistent"), m_gameobject );
			playerdata.connection = playerdata.persistent->AddComponent<PlayerNetwork>();
			m_players[_ID] = playerdata;
			playerdata.name = _name;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::SpawnSpaceShip( const int _playerID )
	{
	
		// Get player data
		auto it = m_players.find( _playerID );
		assert( it !=  m_players.end() );
		PlayerData& playerData = it->second;

		Gameobject * player = GetScene().CreateGameobject( **m_playerPrefab, playerData.persistent );
		player->SetEditorFlags( player->GetEditorFlags() | Gameobject::EditorFlag::NOT_SAVED );
		player->SetName( playerData.name );

		// Set input
		PlayerInput * playerInput = player->GetComponent<PlayerInput>();
		if ( playerInput != nullptr )
		{
			playerInput->SetJoystickID( _playerID );
			playerInput->SetInputType( _playerID < 0 ? PlayerInput::KEYBOARD_MOUSE : PlayerInput::JOYSTICK );
		}
		else
		{
			Debug::Warning( "PlayersManager::AddPlayer : Prefab is missing a PlayerInput component." );
		}

		SpaceShip * playerShip = player->GetComponent<SpaceShip>();
		if ( playerShip != nullptr )
		{
			playerShip->onPlayerDie.Connect( &PlayersManager::OnPlayerDie, this );
		}
		else
		{
			Debug::Warning( "PlayersManager::AddPlayer : Prefab is missing a SpaceShip component." );
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
			m_gameobject->GetScene().DeleteGameobject( _gameobject );
		}
	}

	//================================================================================================================================
	// Removes a player from the scene
	//================================================================================================================================
	void PlayersManager::RemovePlayer( const int _ID )
	{
		assert( m_players.find( _ID ) != m_players.end() );

		PlayerData playerData = m_players[_ID];
		m_players.erase( _ID );
		m_gameobject->GetScene().DeleteGameobject( playerData.persistent );
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
			//RemovePlayer( _joystickID );
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
			if ( pair.second.spaceship != nullptr )
			{
				players.push_back( pair.second.spaceship );
			}			
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