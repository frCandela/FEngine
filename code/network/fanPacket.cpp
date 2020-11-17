#include "network/fanPacket.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Packet::Packet( const PacketTag _tag )
	{
        mTag = _tag;
		mPacket << _tag;
	}

	//========================================================================================================
	//========================================================================================================
	PacketType Packet::ReadType()
	{
		PacketTypeInt intType;
		mPacket >> intType;
		return PacketType( intType );
	}

	//========================================================================================================
	//========================================================================================================
	void Packet::Clear()
	{
		mPacket.clear();
        mTag             = 0;
        mOnlyContainsAck = false;
		mOnFail.Clear();
		mOnSuccess.Clear();
	}

	//========================================================================================================
	// we could make this more efficient with a custom packet/socket library
	// that allows better access to packet data
	//========================================================================================================
	void PacketReplication::Read( Packet& _packet )
	{
		mPacketData.clear();

		// get replication type
		sf::Uint8 replicationTypeInt;
		_packet >> replicationTypeInt;
        mReplicationType = ReplicationType( replicationTypeInt );

		// get data size
		sf::Uint8 dataSize;
		_packet >> dataSize;

		// get data
		for (int i = 0; i < dataSize ; i++)
		{
			sf::Uint8 data;
			_packet >> data; 
			mPacketData << data;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void PacketReplication::Write( Packet& _packet ) const
	{
        fanAssert( mPacketData.getDataSize() < std::numeric_limits<sf::Uint8>::max() );
        fanAssert( mReplicationType != ReplicationType::Count );
 
 		_packet << PacketTypeInt( PacketType::Replication );
 		_packet << sf::Uint8( mReplicationType );
 		_packet << sf::Uint8( mPacketData.getDataSize() );
 		
		sf::Packet packetCpy = mPacketData;
		for( int i = 0; i < (int)mPacketData.getDataSize(); i++ )
		{
			sf::Uint8 data;
			packetCpy >> data;
			_packet << data;
		}
	}
}