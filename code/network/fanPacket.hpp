#pragma once

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Network.hpp"

#include "core/fanSignal.hpp"

namespace fan
{
	struct SingletonComponent;
	struct SingletonComponentInfo;
	class EcsWorld;

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
		, ReplicationSingletonComponents
		, GameState
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
		Signal< HostID, PacketTag > onFail;		// packet was dropped
		Signal< HostID, PacketTag > onSuccess;	// packet was received
		bool onlyContainsAck = false;
	private:
		sf::Packet m_packet;
	};

	//================================================================================================================================
	// server -> client
	// Used to calculate a the connection RTT
	//================================================================================================================================
	struct PacketPing
	{
		void Read( Packet& _packet ) 
		{
			_packet >> roundTripTime;
		}

		void Write( Packet& _packet ) 
		{ 
			_packet << sf::Uint16( PacketType::Ping );
			_packet << roundTripTime;
		}

		float roundTripTime;
	};

	//================================================================================================================================
	// client -> server
	// first packet send from the client to login into the server
	// server should respond with a LoggedIn packet
	//================================================================================================================================
	struct PacketHello
	{
		void Write( Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::Hello );
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
		void Write( Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::LoggedIn );
		}
		void Read( Packet& /*_packet*/ )
		{
		}
	};

	//================================================================================================================================
	// Packet status : server->client
	//================================================================================================================================
	struct PacketGameState
	{
		void Read( Packet& _packet )
		{
			_packet >> frameIndex;
			_packet >> solarEruptionStart;
		}

		void Write( Packet& _packet )
		{
			_packet << sf::Uint16( PacketType::GameState );
			_packet << frameIndex;
			_packet << solarEruptionStart;
		}

		sf::Uint64			frameIndex			= 0;
		sf::Uint64			solarEruptionStart  = 0;
	};

	//================================================================================================================================
	// server->client  Replication packet for singleton components 
	// serialized data is stored in a sf::Packet
	//================================================================================================================================
	struct PacketReplicationSingletonComponents
	{
		//================================================================
		//================================================================
		struct SingletonData
		{
			const SingletonComponentInfo* componentInfo = nullptr;
			SingletonComponent* component = nullptr;
		};		

		void Read( Packet& _packet );
		void Write( Packet& _packet ) const;
		void Generate( const SingletonComponentInfo& _componentInfo, const SingletonComponent& _component );
		void ReplicateOnWorld( EcsWorld& _world );

		sf::Packet packetData;
	};
}