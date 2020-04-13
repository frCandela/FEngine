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
		, START
		, PLAYER_INPUT

		, COUNT
	};

	//================================================================================================================================
	// PacketLogin server<-client
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
	// Packet ACK server<->client
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
	// Packet ping server<->client
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
	// Packet status : server->client
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
	// Packet game start : server->client
	// send to the client when the game is starting
	//================================================================================================================================
	struct PacketStart
	{
		PacketStart() {}

		PacketStart( sf::Packet& _packet )
		{
			_packet >> frameStartIndex;
		}

		sf::Packet ToPacket()
		{
			sf::Packet packet;
			packet << sf::Uint16( PacketType::START );
			packet << frameStartIndex;
			return packet;
		}

		sf::Uint64	frameStartIndex = 0;
	};

	//================================================================================================================================
	// Empty packets
	//================================================================================================================================
// 	struct PacketAckLogin : IPacket<PacketType::ACK_LOGIN> {};
// 	struct PacketAckLogout : IPacket<PacketType::LOGOUT> {};
// 	struct PacketStartGame : IPacket<PacketType::START_GAME> {};
}