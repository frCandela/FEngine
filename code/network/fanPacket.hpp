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
			, Entity
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
		struct InputData
		{
			bool left : 1;
			bool right : 1;
			bool forward : 1;
			bool backward : 1;
			bool boost : 1;
			bool fire : 1;
			sf::Vector2f	orientation;
			FrameIndexNet	frameIndex;
		}; 

		std::vector<InputData> inputs;

		void Read( Packet& _packet )
		{
			sf::Uint8 size;
			_packet >> size;

			inputs.resize( size );
			for( int i = 0; i < size; i++ )
			{
				InputData& inputData = inputs[i];
				_packet >> inputData.frameIndex;
				_packet >> inputData.orientation.x >> inputData.orientation.y;
				sf::Uint8 keyBits;
				_packet >> keyBits;

				inputData.left		= keyBits & (1<<0);
				inputData.right		= keyBits & (1<<1);
				inputData.forward	= keyBits & (1<<2);
				inputData.backward	= keyBits & (1<<3);
				inputData.boost		= keyBits & (1<<4);
				inputData.fire		= keyBits & (1<<5);
			}
		}

		void Write( Packet& _packet ) const
		{
			_packet << PacketTypeInt( PacketType::PlayerInput );
			_packet << sf::Uint8(inputs.size());

			for( int i = 0; i < inputs.size(); i++ )
			{
				const InputData& inputData = inputs[i];
				const sf::Uint8 keyBits =
				inputData.left		<< 0 |
				inputData.right		<< 1 |
				inputData.forward	<< 2 |
				inputData.backward	<< 3 |
				inputData.boost		<< 4 |
				inputData.fire		<< 5;

				_packet << inputData.frameIndex;
				_packet << inputData.orientation.x << inputData.orientation.y;
				_packet << keyBits;
			}
		}
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

		FrameIndexNet	frameIndex = 0;			// the  frame index when creating state		
		btVector3		position;			
		btVector3		orientation;		
		btVector3		velocity;			
		btVector3		angularVelocity;
	};
	
}