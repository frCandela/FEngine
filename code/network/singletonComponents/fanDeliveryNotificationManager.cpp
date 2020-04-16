#include "network/singletonComponents/fanDeliveryNotificationManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"


namespace fan
{
	REGISTER_SINGLETON_COMPONENT( DeliveryNotificationManager );

	//================================================================================================================================
	//================================================================================================================================
	void DeliveryNotificationManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.init = &DeliveryNotificationManager::Init;
		_info.onGui = &DeliveryNotificationManager::OnGui;
		_info.name = "delivery notification manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void DeliveryNotificationManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		DeliveryNotificationManager& deliveryNotificationManager = static_cast<DeliveryNotificationManager&>( _component );
		deliveryNotificationManager.timeoutDuration = 3.f;
		deliveryNotificationManager.hostDatas.clear();
		deliveryNotificationManager.hostDatas.reserve( 4 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DeliveryNotificationManager::CreateHost( const HostID _hostID )
	{
		if( _hostID >= hostDatas.size() )
		{
			hostDatas.resize( (size_t)_hostID + 1 );
		}
		hostDatas[_hostID] = {};
	}

	//================================================================================================================================
	// Ensure packets are never processed out of order.Old packets arriving after newer packets are dropped.
	// Return true if the packet is valid and ready to be processed, false otherwise
	//================================================================================================================================
	bool DeliveryNotificationManager::ValidatePacket( Packet& _packet, const HostID _hostID )
	{
		if( _packet.onlyContainsAck ) return true;

		HostData& hostData = hostDatas[_hostID];
		if( _packet.tag == hostData.expectedPacketTag )	// packet is perfect \o/
		{
			Debug::Log() << "received " << _packet.tag << Debug::Endl();
			hostData.expectedPacketTag++;
			hostData.pendingAck.push_back( _packet.tag );
			return true;
		}
		else if( _packet.tag < hostData.expectedPacketTag ) // silently drop old packet.
		{
			return false;
		}
		else //we missed some packets
		{
			assert( _packet.tag > hostData.expectedPacketTag );
			hostData.expectedPacketTag = _packet.tag + 1;
			hostData.pendingAck.push_back( _packet.tag );
			return true;
		}
	}

	//================================================================================================================================
	// Registers the packet as an inFlightPacket.
	// allows timeout and delivery notification to be issued
	//================================================================================================================================
	void DeliveryNotificationManager::RegisterPacket( Packet& _packet, const HostID _hostID )
	{
		HostData& hostData = hostDatas[_hostID];

		if( _packet.onlyContainsAck )
		{
			hostData.nextPacketTag--;
			return;
		}
		
		InFlightPacket inFlightPacket;
		inFlightPacket.tag = _packet.tag;
		inFlightPacket.onFailure = _packet.onFail;
		inFlightPacket.onSuccess = _packet.onSuccess;
		inFlightPacket.timeDispatch = Time::Get().ElapsedSinceStartup();		
		hostData.inFlightPackets.push( inFlightPacket );

		Debug::Log() << "sending " << inFlightPacket.tag << Debug::Endl();
	}

	//================================================================================================================================
	// Process incoming acknowledgments and notify connected modules about which packets were received or dropped
	//================================================================================================================================
	void DeliveryNotificationManager::Receive( const PacketAck& _packetAck, const HostID _hostID )
	{
		HostData& hostData = hostDatas[_hostID];

		int ackIndex = 0;
		while( ackIndex < _packetAck.tags.size() &&	!hostData.inFlightPackets.empty() )
		{
			const PacketTag ackPacketTag = _packetAck.tags[ackIndex];
			/*const*/ InFlightPacket& inFlightPacket = hostData.inFlightPackets.front();
			if( inFlightPacket.tag == ackPacketTag ) // packet was received ! \o/
			{
				Debug::Log() << "receive ack " << ackPacketTag << Debug::Endl();
				inFlightPacket.onSuccess.Emmit( _hostID );
				hostData.inFlightPackets.pop();	
				ackIndex++;
			}
			else if( inFlightPacket.tag < ackPacketTag ) // packet was dropped or too old
			{
				Debug::Log() << "drop " << ackPacketTag << Debug::Endl();
				inFlightPacket.onFailure.Emmit( _hostID ); 
				hostData.inFlightPackets.pop();
			}
			else // inFlightPacket was already removed (maybe from timeout)	
			{				
				assert( inFlightPacket.tag > ackPacketTag );							
				ackIndex++;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DeliveryNotificationManager::ProcessTimedOutPackets()
	{
		const double timeoutTime = Time::Get().ElapsedSinceStartup() - timeoutDuration;
		for( HostID hostID = 0; hostID < hostDatas.size(); ++hostID )
		{
			HostData& hostData = hostDatas[hostID];

			//packets are sorted, so all timed out packets must be at front
			while( !hostData.inFlightPackets.empty() )
			{
				InFlightPacket& inFlightPacket = hostData.inFlightPackets.front();
				if( inFlightPacket.timeDispatch < timeoutTime ) // packet timed out
				{
					Debug::Log() << "timeout " << inFlightPacket.tag << Debug::Endl();
					inFlightPacket.onFailure.Emmit( hostID );
					hostData.inFlightPackets.pop();
				}
				else //no packets beyond could be timed out
				{
					
					break;
				}
			}
		}
	}

	//================================================================================================================================
	// Send out an acknowledgment for each validated packet 
	//================================================================================================================================
	void DeliveryNotificationManager::SendAck( Packet& _packet, const HostID _hostID )
	{
		HostData& hostData = hostDatas[_hostID];
		if( !hostData.pendingAck.empty() )
		{
			PacketAck packetAck;
			packetAck.tags = hostData.pendingAck;
			packetAck.Write( _packet );
			hostData.pendingAck.clear();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DeliveryNotificationManager::OnGui( SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			DeliveryNotificationManager& deliveryNotificationManager = static_cast<DeliveryNotificationManager&>( _component );
			
			ImGui::DragFloat( "timeout", &deliveryNotificationManager.timeoutDuration );

			for (int i = 0; i < deliveryNotificationManager.hostDatas.size(); i++)
			{
				HostData& hostData = deliveryNotificationManager.hostDatas[i];

				ImGui::Text( "Host %d:", i );
				ImGui::Text( "next packet tag:       %d", hostData.nextPacketTag );
				ImGui::Text( "expected packet tag:   %d", hostData.expectedPacketTag );
				ImGui::Text( "num pending ack:       %d", hostData.pendingAck.size() );
				ImGui::Text( "num in flight packets: %d", hostData.inFlightPackets.size() );
				ImGui::Spacing(); ImGui::Spacing();
			}
		}ImGui::Unindent(); ImGui::Unindent();
	}
}