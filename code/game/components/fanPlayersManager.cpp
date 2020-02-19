#include "game/components/fanPlayersManager.hpp"

#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/network/fanGameClient.hpp"
#include "core/input/fanJoystick.hpp"

namespace fan
{
	REGISTER_TYPE_INFO( PlayersManager, TypeInfo::Flags::EDITOR_COMPONENT, "game/managers/" )

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::Start()
	{
		REQUIRE_TRUE( *m_playerPrefab != nullptr, "PlayersManager : missing player prefab " )

			if ( !GetScene().IsServer() )
			{
				// Callback used to spawn future joystick players
				Joystick::Get().onJoystickConnect.Connect( &PlayersManager::OnJoystickConnect, this );

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

				AddPlayer( s_mousePlayerID, "mouse_player" );
			}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::Stop()
	{
		if ( !GetScene().IsServer() )
		{
			Joystick::Get().onJoystickConnect.Disconnect( &PlayersManager::OnJoystickConnect, this );
		}

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
			assert( m_players.find( _ID ) == m_players.end() );
			PlayerData playerdata;
			playerdata.persistent = GetScene().CreateGameobject( _name + std::string( "_persistent" ), m_gameobject );
			m_players[ _ID ] = playerdata;
			onAddPlayer.Emmit( playerdata.persistent );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::SpawnSpaceShips()
	{
		for ( auto pair : m_players )
		{
			const int playerID = pair.first;

			// Get player data
			auto it = m_players.find( playerID );
			assert( it != m_players.end() );
			PlayerData& playerData = it->second;


			assert( playerData.persistent );
			Gameobject* player = GetScene().CreateGameobject( **m_playerPrefab, playerData.persistent );
			player->SetEditorFlags( player->GetEditorFlags() | Gameobject::EditorFlag::NOT_SAVED );
			player->SetName( playerData.persistent->GetName() );

			// Set input
			PlayerInput* playerInput = player->GetComponent<PlayerInput>();
			if ( playerInput != nullptr )
			{
				playerInput->SetJoystickID( playerID );
				playerInput->SetInputType( playerID < 0 ? PlayerInput::KEYBOARD_MOUSE : PlayerInput::JOYSTICK );
			}
			else
			{
				Debug::Warning( "PlayersManager::AddPlayer : Prefab is missing a PlayerInput component." );
			}

			SpaceShip* playerShip = player->GetComponent<SpaceShip>();
			if ( playerShip != nullptr )
			{
				playerShip->onPlayerDie.Connect( &PlayersManager::OnPlayerDie, this );
			}
			else
			{
				Debug::Warning( "PlayersManager::AddPlayer : Prefab is missing a SpaceShip component." );
			}
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayersManager::OnPlayerDie( Gameobject* _gameobject )
	{
		// Removes dead player from the game
		PlayerInput* input = _gameobject->GetComponent<PlayerInput>();
		if ( input != nullptr )
		{
			RemovePlayer( input->GetJoystickID() );
			m_gameobject->GetScene().DeleteGameobject( _gameobject );
		}
	}

	//================================================================================================================================
	// Removes a player from the scene
	//================================================================================================================================
	void PlayersManager::RemovePlayer( const int _ID )
	{
		assert( m_players.find( _ID ) != m_players.end() );

		PlayerData playerData = m_players[ _ID ];
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
			AddPlayer( _joystickID, ss.str() );
		}
	}

	//================================================================================================================================
	// Returns an array of connected players data (copy)
	//================================================================================================================================
	std::vector< Gameobject* > PlayersManager::GetPlayers() const
	{
		std::vector< Gameobject* > players;
		players.reserve( m_players.size() );
		for ( auto& pair : m_players )
		{
			players.push_back( pair.second.persistent );
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
			ImGui::FanPrefab( "player prefab", m_playerPrefab );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayersManager::Load( const Json& _json )
	{
		Actor::Load( _json );
		Serializable::LoadPrefabPtr( _json, "player_prefab", m_playerPrefab );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PlayersManager::Save( Json& _json ) const
	{
		Actor::Save( _json );
		Serializable::SavePrefabPtr( _json, "player_prefab", m_playerPrefab );
		return true;
	}
}