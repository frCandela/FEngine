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
		sf::Uint16 intType;
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
	// Generates the packet data from a list of singleton
	//================================================================================================================================
	void PacketReplicationSingletonComponents::Generate( const SingletonComponentInfo& _componentInfo, const SingletonComponent& _component )
	{
		packetData.clear();
		packetData << sf::Uint32( _componentInfo.staticIndex );
		_componentInfo.netSave( _component, packetData );
	}

	//================================================================================================================================
	// Generates the packet data from a list of singleton
	//================================================================================================================================
	void PacketReplicationSingletonComponents::ReplicateOnWorld( EcsWorld& _world )
	{
		sf::Uint32 staticIndex;
		packetData >> staticIndex;

		SingletonComponent& singleton = _world.GetSingletonComponent( staticIndex );
		const SingletonComponentInfo& info = _world.GetSingletonComponentInfo( staticIndex );
		info.netLoad( singleton, packetData );

		assert( packetData.endOfPacket() );
	}

	//================================================================================================================================
	// we could make this more efficient with a custom packet/socket library that allows better access to packet data
	//================================================================================================================================
	void PacketReplicationSingletonComponents::Read( Packet& _packet )
	{
		packetData.clear();

		sf::Uint16 dataSize;
		_packet >> dataSize;
		for (int i = 0; i < dataSize ; i++)
		{
			sf::Uint8 data;
			_packet >> data; 
			packetData << data;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PacketReplicationSingletonComponents::Write( Packet& _packet ) const
	{
		assert( packetData.getDataSize() < std::numeric_limits<sf::Uint16>::max() );
		

		_packet << sf::Uint16( PacketType::ReplicationSingletonComponents );
		_packet << sf::Uint16( packetData.getDataSize() );
		
		sf::Packet packetCpy = packetData;
		for( int i = 0; i < packetData.getDataSize(); i++ )
		{
			sf::Uint8 data;
			packetCpy >> data;
			_packet << data;
		}
	}
}