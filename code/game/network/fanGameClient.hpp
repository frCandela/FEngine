#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "network/fanUDPSocket.hpp"

namespace fan
{
	class PlayersManager;
	class GameManager;

	//================================================================================================================================
	//================================================================================================================================
	class GameClient// : public Actor
	{
	public:
		enum NetPlayerState { PLAYER_NONE, PLAYER_CONNECTING, PLAYER_CONNECTED };
		enum ClientState { CLIENT_NONE, CLIENT_CONNECTING, CLIENT_CONNECTED };

		//================================================================	
		//================================================================
// 		struct NetPlayerData
// 		{
// 			Gameobject* playerPersistent = nullptr;
// 			NetPlayerState  state = PLAYER_NONE;
// 		};


		void			OnGui() /*override*/;
		//ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		// ISerializable
		bool Load( const Json& _json ) /*override*/;
		bool Save( Json& _json ) const /*override*/;

		void Start() /*override*/;
		void Stop() /*override*/;
		void Update( const float _delta ) /*override*/;
		void LateUpdate( const float _delta ) /*override*/;

// 		const std::vector<NetPlayerData >& GetNetPlayers() const { return m_netPlayers; }

	protected:
		void OnAttach() /*override*/;
		void OnDetach() /*override*/;

	private:
// 		UDPSocket		m_socket;
// 		ClientState		m_state;
// 		Port			m_serverPort;
// 		sf::IpAddress	m_serverIp;
// 
// 		float			m_timer = 0.f;
// 
// 		GameManager* m_gameManager;
// 		PlayersManager* m_playersManager;
// 
// 		std::vector< NetPlayerData > m_netPlayers;
// 
// 		void OnAddPlayer( Gameobject* _playerPersistent );
// 		void ConnectToServer( const Port _serverPort, const sf::IpAddress _ip );
// 		void Receive();
// 		NetPlayerData* FindNetPlayer( const Gameobject* _gameobject );
	};
}