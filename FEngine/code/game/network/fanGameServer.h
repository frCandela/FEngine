#pragma once

#include "scene/actors/fanActor.h"
#include "network/fanUDPSocket.h"

namespace fan
{
	class GameManager;
	class PlayersManager;
	struct PacketLogin;

	//================================================================================================================================
	//================================================================================================================================
	class GameServer : public Actor
	{
	public:
		enum ServerState {  SERVER_NONE, SERVER_WAITING_PLAYERS };
		enum ClientState {  CLIENT_WAITING, CLIENT_CONNECTED };

		Signal< const int, const std::string& > onClientConnected;

		//================================================================
		//================================================================
		struct ClientData
		{
			sf::IpAddress	ipAdress;
			Port			port;
			std::string		name;
			ClientState		state = ClientState::CLIENT_WAITING;
			float			ping = -1.f;
			float			lastResponse = 0.f;
		};

		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		// ISerializable
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void Start() override;
		void Stop() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;

		const UDPSocket&				GetSocket() const	{ return m_socket;  }
		const std::vector<ClientData >& GetClients() const	{ return m_clients; }

		void StartGame();
		
		DECLARE_TYPE_INFO( GameServer, Component );

	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		UDPSocket   m_socket;
		ServerState	m_state = SERVER_NONE;

		std::vector<ClientData > m_clients;

		GameManager		* m_gameManager;
		PlayersManager  * m_playersManager;

		void		 Receive();
		bool		 Send( sf::Packet _packet, const ClientData& _client );
		void		 Broadcast( sf::Packet _packet );
		void		 UpdateClient( ClientData& _client, const float _delta );
		void		 ProcessPacket(const sf::IpAddress& _ip, const Port& _port, sf::Packet& _packet );
		ClientData * FindClient( const sf::IpAddress& _ip, const Port& _port );
		ClientData * AddClient( const sf::IpAddress& _ip, const Port& _port, const PacketLogin& _loginInfo );
		void		 RemoveClient( ClientData& _client );
		void		 ClearClients();

	};
}