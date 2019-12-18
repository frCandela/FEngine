#pragma once

#include "scene/actors/fanActor.h"
#include "network/fanUDPSocket.h"

namespace fan
{
	class PlayerInput;

	//================================================================================================================================
	//================================================================================================================================
	class GameClient : public Actor
	{
	public:
		enum ClientState { NONE, CONNECTING, CONNECTED };

		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		// ISerializable
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void Start() override;
		void Stop() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;
		
		DECLARE_TYPE_INFO( GameClient, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		UDPSocket		m_socket;
		ClientState		m_state;
		Port			m_serverPort;
		sf::IpAddress	m_serverIp;

		float			m_timer = 0.f;

		void ConnectToServer( const Port _serverPort, const sf::IpAddress _ip );
		void Receive();
	};
}