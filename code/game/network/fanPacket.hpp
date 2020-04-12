#pragma once

#include "SFML/System.hpp"
#include "SFML/Network.hpp"

#include <iostream>

namespace fan
{
	enum PacketType
	{
		  PING = 0
		, ACK
		, LOGIN
		, LOGOUT
		, STATUS
		, PLAYER_INPUT
		, START_GAME

		, COUNT
	};

	//================================================================================================================================
	// PacketLogin
	//================================================================================================================================
	struct PacketLogin
	{
		sf::Packet ToPacket()
		{
			sf::Packet packet;
			packet << sf::Uint16( PacketType::LOGIN );
			packet << name;
			return packet;
		}

		PacketLogin() {}
		PacketLogin( sf::Packet& _packet )		
		{
			_packet >> name;
		}

		std::string name = "";
	};

	//================================================================================================================================
	// Packet ACK
	//================================================================================================================================
	struct PacketACK
	{
		sf::Packet ToPacket()
		{
			sf::Packet packet;
			packet << sf::Uint16( PacketType::ACK );
			packet << sf::Uint16( ackType );
			return packet;
		}

		PacketACK() {}
		PacketACK( sf::Packet& _packet )
		{
			sf::Uint16 intType;
			_packet >> intType;
			ackType = PacketType( intType );
		}

		PacketType ackType = PacketType::COUNT;
	};


	//================================================================================================================================
	// Packet ping
	//================================================================================================================================
	struct PacketPing
	{
		PacketPing(){}

		PacketPing( sf::Packet& _packet )
		{
			_packet >> time;
		}

		sf::Packet ToPacket()
		{
			sf::Packet packet;
			packet << sf::Uint16( PacketType::PING );
			packet << time;
			return packet;
		}

		double time = -1.f;
	};

	//================================================================================================================================
	// Packet status -> client status send from the server to the client
	//================================================================================================================================
	struct PacketStatus
	{
		PacketStatus() {}

		PacketStatus( sf::Packet& _packet )
		{
			_packet >> roundTripDelay;
			_packet >> frameIndex;
		}

		sf::Packet ToPacket()
		{
			sf::Packet packet;
			packet << sf::Uint16( PacketType::STATUS );
			packet << roundTripDelay;
			packet << frameIndex;
			return packet;
		}

		float		roundTripDelay = -1.f;
		sf::Uint64	frameIndex = 0;
	};
	

	//================================================================================================================================
	//================================================================================================================================
/*	template< PacketType _Type >
	struct IPacket
	{
		virtual void LoadFrom( sf::Packet&  ) {};

		sf::Packet ToPacket()
		{
			ExportTo( packet );
			return packet;
		}

	protected:
		virtual void ExportTo( sf::Packet& _packet ) const
		{
			_packet << sf::Uint16( s_type );
		}

		static const PacketType s_type = _Type;
	};*/


	//================================================================================================================================
	// Empty packets
	//================================================================================================================================
// 	struct PacketAckLogin : IPacket<PacketType::ACK_LOGIN> {};
// 	struct PacketAckLogout : IPacket<PacketType::LOGOUT> {};
// 	struct PacketStartGame : IPacket<PacketType::START_GAME> {};
}