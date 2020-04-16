#pragma once

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Network.hpp"

#include "core/fanSignal.hpp"

namespace fan
{
	struct Client;
	using HostID = int;	
	using IpAddress = sf::IpAddress;
	using Port = unsigned short;
	using PacketTag = sf::Uint32;		// may change to uint16 on release

	enum class PacketType
	{
		  Ping = 0
		, Ack
		, Hello
		, LoggedIn
		, COUNT
	};

	//================================================================================================================================
	//================================================================================================================================
	struct Packet
	{
		Packet(){} // for receiving only
		Packet( const PacketTag _tag );

		template <typename T>
		Packet& operator<<( T _value )
		{
			m_packet << _value;
			return *this;
		}

		template <typename T>
		Packet& operator>>( T& _value )
		{
			m_packet >> _value;
			return *this;
		}

		bool		EndOfPacket() const { return m_packet.endOfPacket(); }
		sf::Packet& ToSfml() { return m_packet; }
		void		Clear();
		PacketType  ReadType();
		size_t		GetSize() const{ return m_packet.getDataSize(); }

		PacketTag		 tag;
		Signal< HostID > onFail;	// packet was dropped
		Signal< HostID > onSuccess;	// packet was received
		bool onlyContainsAck = false;
	private:
		sf::Packet m_packet;
	};

	//================================================================================================================================
	// server -> client
	// Used to calculate a the connection RTT and keep track of disconnected clients
	//================================================================================================================================
	struct PacketPing
	{
		void Load( Packet& /*_packet*/ ) {}

		void Save( Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::Ping );
		}
	};

	//================================================================================================================================
	// client -> server
	// first packet send from the client to login into the server
	// server should respond with a LoggedIn packet
	//================================================================================================================================
	struct PacketHello
	{
		void Save( Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::Hello );
			_packet << name;
		}
		void Load( Packet& _packet )
		{
			_packet >> name;
		}

		std::string name = "";
	};

	//================================================================================================================================
	// server -> client
	// Packet send from the server to the client after a Hello to acknowledge successful login
	//================================================================================================================================
	struct PacketLoginSuccess
	{
		void Save( Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::LoggedIn );
		}
		void Load( Packet& /*_packet*/ )
		{
		}
	};

// 	//================================================================================================================================
// 	// Packet status : server->client
// 	//================================================================================================================================
// 	struct PacketStatus
// 	{
// 		void Load( Packet& _packet )
// 		{
// 			_packet >> roundTripDelay;
// 			_packet >> frameIndex;
// 		}
// 
// 		void Save( Packet& _packet )
// 		{
// 			_packet << sf::Uint16( PacketType::Status );
// 			_packet << roundTripDelay;
// 			_packet << frameIndex;
// 		}
// 
// 		float		roundTripDelay = -1.f;
// 		sf::Uint64	frameIndex = 0;
// 	};
// 	
// 	//================================================================================================================================
// 	// Packet game start : server->client
// 	// send to the client when the game is starting
// 	//================================================================================================================================
// 	struct PacketStart
// 	{
// 		void Load( Packet& _packet )
// 		{
// 			_packet >> frameStartIndex;
// 		}
// 
// 		void Save( Packet& _packet )
// 		{
// 			_packet << sf::Uint16( PacketType::START );
// 			_packet << frameStartIndex;
// 		}
// 
// 		sf::Uint64	frameStartIndex = 0;
// 	};
}