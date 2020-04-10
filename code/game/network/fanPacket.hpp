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
		, PLAYER_INPUT
		, START_GAME
		, COUNT
	};

	//================================================================================================================================
	// PacketLogin
	//================================================================================================================================
	struct PacketLogin
	{
		void SaveTo( sf::Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::LOGIN );
			_packet << name;
		}

		void LoadFrom( sf::Packet& _packet )
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
		void SaveTo( sf::Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::ACK);
			_packet << sf::Uint16( ackType );
		}

		void LoadFrom( sf::Packet& _packet )
		{
			sf::Uint16 intType;
			_packet >> intType;
			ackType = PacketType( intType );
		}

		PacketType ackType = PacketType::COUNT;
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