#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanUdpSocket.hpp"
#include "network/fanPacket.hpp"
#include <queue>

namespace fan
{
	class EcsWorld;

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
	// Uniquely identify and tag each packet send out
	// Send out an acknowledgment for each validated packet 
	// Process incoming acknowledgments and notify connected modules about which packets were received or dropped
	// Ensure packets are never processed out of order. Old packets arriving after newer packets are dropped.
	//
	// HostID is an unique ID used to identify a remote host and store its state
	// The server has multiple hosts, on for each client
	// The client has a single host, the server. Its id is always zero
	//================================================================================================================================	
	struct DeliveryNotificationManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		void CreateHost( const HostID _hostID = 0 );
		void DeleteHost( const HostID _hostID = 0 );

		PacketTag GetNextPacketTag( const HostID _hostID = 0 ) { return hostDatas[_hostID].nextPacketTag++; }
		void RegisterPacket( Packet& _packet, const HostID  _hostID = 0 );
		bool ValidatePacket( Packet& _packet, const HostID  _hostID = 0 );
		void ProcessTimedOutPackets();

		void Receive( const PacketAck& _packetAck,  const HostID _hostID = 0 );
		void Write   ( Packet& _packet,			const HostID _hostID = 0 );

		//================================================================
		//================================================================
		struct InFlightPacket
		{
			PacketTag			tag;
			double				timeDispatch;
			Signal< HostID, PacketTag >	onFailure;
			Signal< HostID, PacketTag >	onSuccess;
		};

		//================================================================
		// @todo change pendingAck into a range to gain space
		//================================================================
		struct HostData
		{
			PacketTag					 nextPacketTag = 0;		// the tag of the next packet being send
			PacketTag					 expectedPacketTag = 0;	// the expected tag of the next received packet 
			std::vector<PacketTag>		 pendingAck;			// validated packets waiting for ack dispatch
			std::queue< InFlightPacket > inFlightPackets;		// list of packets pending success/drop status
		};

		float					timeoutDuration; // time after which a packet is considered dropped
		std::vector< HostData > hostDatas;		 // list of hosts data. Order in the array correspond to ClientIDs
	};
}