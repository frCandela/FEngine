#pragma once

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Network.hpp"

#include "core/fanSignal.hpp"

namespace fan
{
	struct Client;
	using HostID = int;	
	using NetID = sf::Uint32;
	using IpAddress = sf::IpAddress;
	using Port = unsigned short;
	using PacketTag = sf::Uint32;		// may change to uint16 on release
	using PacketTypeInt = sf::Uint8;	// the sf integer type encoding the PacketType enum

	// all the types of packets of the network engine
	enum class PacketType
	{
		  Ping = 0		// server calculates the RTT &sends it to the client
		, Ack			// packet reception acknowledgment 
		, Hello			// first presentation of the client to the server for logging in
		, LoggedIn		// server informs client that login was successful
		, Replication	// replication of data on the client's world
		, COUNT			
	}; 
	static_assert( int( PacketType::COUNT ) < std::numeric_limits<PacketTypeInt>::max() );

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
		Signal< HostID, PacketTag > onFail;		// packet was dropped
		Signal< HostID, PacketTag > onSuccess;	// packet was received
		bool onlyContainsAck = false;
	private:
		sf::Packet m_packet;
	};

	//================================================================================================================================
	// server -> client then client -> server	
	// Used to calculate a the connection RTT & frame index synch delta with the client
	//================================================================================================================================
	struct PacketPing
	{
		void Read( Packet& _packet ) 
		{
			_packet >> serverFrame;
			_packet >> clientFrame;
			_packet >> previousRtt;
		}

		void Write( Packet& _packet ) const
		{ 
			_packet << PacketTypeInt( PacketType::Ping );
			_packet << serverFrame;
			_packet << clientFrame;
			_packet << previousRtt;
		}

		sf::Uint64 serverFrame;	// frame index of the server when sending the packet
		sf::Uint64 clientFrame; // frame index of the client when sending back the packet
		float previousRtt;		// client rtt from the previous ping
	};

	//================================================================================================================================
	// client -> server
	// first packet send from the client to login into the server
	// server should respond with a LoggedIn packet
	//================================================================================================================================
	struct PacketHello
	{
		void Write( Packet& _packet ) const
		{
			_packet << PacketTypeInt( PacketType::Hello );
			_packet << name;
		}
		void Read( Packet& _packet )
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
		void Write( Packet& _packet ) const
		{
			_packet << PacketTypeInt( PacketType::LoggedIn );
		}
		void Read( Packet& /*_packet*/ )
		{
		}
	};

	//================================================================================================================================
	// server->client  Replication packet for singleton components, components & RPC
	// serialized data is stored in a sf::Packet, it must be generated from the server replication manager
	//================================================================================================================================
	struct PacketReplication
	{
		void Read( Packet& _packet );
		void Write( Packet& _packet ) const;
		
		enum class ReplicationType
		{
			  SingletonComponent
			, Component
			, RPC
			, Count
		};

		ReplicationType replicationType = ReplicationType::Count;
		sf::Packet		packetData;	
	};
}