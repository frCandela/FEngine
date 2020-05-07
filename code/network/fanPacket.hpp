#pragma once

#include <iostream>
#include <limits>
#include <assert.h>
#include <type_traits>

#include "SFML/System.hpp"
#include "SFML/Network.hpp"

#include "core/fanSignal.hpp"
#include "core/fanDebug.hpp"
#include "bullet/LinearMath/btVector3.h"

namespace fan
{
	struct Client;
	using NetID = sf::Uint32;
	using IpAddress = sf::IpAddress;
	using Port = unsigned short;
	using PacketTag = sf::Uint32;		// may change to uint16 on release
	using PacketTypeInt = sf::Uint8;	// the sf integer type encoding the PacketType enum
	using FrameIndexNet = sf::Uint32;

	// all the types of packets of the network engine
	enum class PacketType
	{
		Ping = 0			// server calculates the RTT &sends it to the client
		, Ack				// packet reception acknowledgment 
		, Hello				// first presentation of the client to the server for logging in
		, Disconnect		// packet sent/received when the player disconnects from the server
		, LoggedIn			// server informs client that login was successful
		, Replication		// replication of data on the client's world
		, PlayerInput		// client input ring buffer sent to the server
		, PlayerGameState	// the game state of one player at a specific frame
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
		Signal< PacketTag > onFail;		// packet was dropped
		Signal< PacketTag > onSuccess;	// packet was received
		bool onlyContainsAck = false;
	private:
		sf::Packet m_packet;
	};

	//================================================================================================================================
	//================================================================================================================================
	struct PacketAck
	{
		void Write( Packet& _packet )
		{
			_packet << PacketTypeInt( PacketType::Ack );
			_packet << sf::Uint16( tags.size() );
			for( PacketTag tag : tags )
			{
				_packet << tag;
			}
		}
		void Read( Packet& _packet )
		{
			sf::Uint16 size;
			_packet >> size;
			tags.resize( size );
			for( int i = 0; i < size; i++ )
			{
				_packet >> tags[i];
			}
		}

		std::vector<PacketTag> tags;
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

		FrameIndexNet serverFrame;	// frame index of the server when sending the packet
		FrameIndexNet clientFrame; // frame index of the client when sending back the packet
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
	// client -> server when the client disconnects
	// server -> client when the server shuts down
	//================================================================================================================================
	struct PacketDisconnect
	{
		void Write( Packet& _packet ) const
		{
			_packet << PacketTypeInt( PacketType::Disconnect );
		}
		void Read( Packet& /*_packet*/ ){}
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

	//================================================================================================================================
	//================================================================================================================================
	struct PacketInput
	{

		enum AxisBits
		{
			Zero = 0
			, Positive = 1 << 0
			, Negative = 1 << 1
			, Mask	   = 0b11
		};

		void Read( Packet& _packet )
		{
			_packet >> frameIndex;
			_packet >> orientation[0] >> orientation[1] >> orientation[2];
			_packet >> left;
			_packet >> forward;
			_packet >> boost;
			_packet >> fire;

			// decode input bits to floats
// 			sf::Uint8 inputsBits;
// 			_packet >> inputsBits;
// 			float* values[4] = { &left, &forward, &boost, &fire };
// 			for( int i = 0; i < 4; i++ )
// 			{
// 				const uint32_t valueBits = ( inputsBits >> i * 2 ) & Mask;
// 				( *values[i] ) = valueBits == Positive ? 1.f : ( valueBits == Negative ? -1.f : 0.f );
// 			}
		}

		void Write( Packet& _packet ) const
		{
			// encode positive/negative/zero input floats into bits that fit into one byte
// 			sf::Uint8 inputsBits = 0;
// 			float values[4] = { left, forward, boost, fire };
// 			for (int i = 0; i < 4; i++)
// 			{
// 				const float value = values[i];
// 				const uint32_t valueBits = value > 0.f ? Positive : ( value < 0.f ? Negative : Zero );
// 				inputsBits |= valueBits << i * 2;
// 			}

			_packet << PacketTypeInt( PacketType::PlayerInput );
			_packet << frameIndex;
			_packet << orientation[0] << orientation[1] << orientation[2];
			_packet << left;
			_packet << forward;
			_packet << boost;
			_packet << fire;
		}

		FrameIndexNet	frameIndex;	 // the  frame index when creating the input
		btVector3		orientation; // orientation of the ship
		float			left;		 // left/right key pressed ( strafing )
		float			forward;	 // forward or backward
		float			boost;		 // shift to go faster
		float			fire;		 // firing in front of the ship
	};

	//================================================================================================================================
	//================================================================================================================================
	struct PacketPlayerGameState
	{
		void Read( Packet& _packet )
		{
			_packet >> frameIndex;
			_packet >> position[0]			>> position[1]			>> position[2];
			_packet >> orientation[0]		>> orientation[1]		>> orientation[2];
			_packet >> velocity[0]			>> velocity[1]			>> velocity[2];
			_packet >> angularVelocity[0]	>> angularVelocity[1]	>> angularVelocity[2];
		}

		void Write( Packet& _packet ) const
		{
			_packet << PacketTypeInt( PacketType::PlayerGameState );
			_packet << frameIndex;
			_packet << position[0]			<< position[1]			<< position[2];
			_packet << orientation[0]		<< orientation[1]		<< orientation[2];
			_packet << velocity[0]			<< velocity[1]			<< velocity[2];
			_packet << angularVelocity[0]	<< angularVelocity[1]	<< angularVelocity[2];
		}

		bool operator==( const PacketPlayerGameState& _other ) const
		{
			return    frameIndex == _other.frameIndex &&
				( position			- _other.position			).fuzzyZero() &&
				( orientation		- _other.orientation		).fuzzyZero() &&
				( velocity			- _other.velocity			).fuzzyZero() &&
				( angularVelocity	- _other.angularVelocity	).fuzzyZero();
		}
		bool operator!=( const PacketPlayerGameState& _other ) const { return !( *this == _other ); }

		FrameIndexNet	frameIndex;			// the  frame index when creating state		
		btVector3		position;			
		btVector3		orientation;		
		btVector3		velocity;			
		btVector3		angularVelocity;
	};
	
}