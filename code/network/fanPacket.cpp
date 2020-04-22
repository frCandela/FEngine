#include "network/fanPacket.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Packet::Packet( const PacketTag _tag )
	{
		tag = _tag;
		m_packet << _tag;
	}

	//================================================================================================================================
	//================================================================================================================================
	PacketType Packet::ReadType()
	{
		PacketTypeInt intType;
		m_packet >> intType;
		return PacketType( intType );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Packet::Clear()
	{
		m_packet.clear();
		tag = 0;
		onlyContainsAck = false;
		onFail.Clear();
		onSuccess.Clear();
	}

	//================================================================================================================================
	// we could make this more efficient with a custom packet/socket library that allows better access to packet data
	//================================================================================================================================
	void PacketReplication::Read( Packet& _packet )
	{
		packetData.clear();

		// get replication type
		sf::Uint8 replicationTypeInt;
		_packet >> replicationTypeInt;
		replicationType = ReplicationType( replicationTypeInt );	

		// get data size
		sf::Uint8 dataSize;
		_packet >> dataSize;

		// get data
		for (int i = 0; i < dataSize ; i++)
		{
			sf::Uint8 data;
			_packet >> data; 
			packetData << data;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PacketReplication::Write( Packet& _packet ) const
	{
 		assert( packetData.getDataSize() < std::numeric_limits<sf::Uint8>::max() );		
 		assert( replicationType != ReplicationType::Count );
 
 		_packet << PacketTypeInt( PacketType::Replication );
 		_packet << sf::Uint8( replicationType );
 		_packet << sf::Uint8( packetData.getDataSize() );
 		
		sf::Packet packetCpy = packetData;
		for( int i = 0; i < packetData.getDataSize(); i++ )
		{
			sf::Uint8 data;
			packetCpy >> data;
			_packet << data;
		}
	}
}