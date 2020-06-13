#include "network/components/fanClientRPC.hpp"

#include <sstream>
#include "ecs/fanEcsWorld.hpp"
#include "network/singletons/fanSpawnManager.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientRPC::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &ClientRPC::OnGui;
		_info.name = "RPC manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRPC::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientRPC& rpc = static_cast<ClientRPC&>( _component );
		rpc.nameToRPCTable.clear();
		rpc.onShiftFrameIndex.Clear();
		rpc.onSpawnClientShip.Clear();
		rpc.onSpawn.Clear();
		rpc.RegisterRPCs();
	}

	//================================================================================================================================
	// Registers  RPC unwrap functions
	// A unwrap function must decode a packet and run the corresponding procedure
	//================================================================================================================================
	void ClientRPC::RegisterRPCs( )
	{
		nameToRPCTable.clear();
		nameToRPCTable[ 'SYNC' ] = &ClientRPC::UnwrapShiftClientFrame ;
		nameToRPCTable[ 'SPAN' ] = &ClientRPC::UnwrapSpawn ;
		nameToRPCTable[ 'SPWN' ] = &ClientRPC::UnwrapSpawnClientShip ;
	}

	//================================================================================================================================
	// Runs a procedure from an incoming rpc packet data
	//================================================================================================================================
	void ClientRPC::TriggerRPC( sf::Packet& _packet )
	{
		RpcId rpcID;
		_packet >> rpcID;

		const RpcUnwrapFunc function =  nameToRPCTable[rpcID];
		( ( *this ).*( function ) )( _packet );
	}

	//================================================================================================================================
	// SynchClientFrame RPC
	//================================================================================================================================
	PacketReplication ClientRPC::RPCShiftClientFrame( const int _framesDelta )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << RpcId( 'SYNC' );
		packet.packetData << sf::Int32(_framesDelta);

		return packet;
	}

	//================================================================================================================================
	// Must be connected to the SpawnManager, copy packet data for future spawning
	//================================================================================================================================
	void ClientRPC::UnwrapSpawn( sf::Packet& _packet )
	{
		FrameIndexNet frameIndex;
		_packet >> frameIndex;
		onSpawn.Emmit( frameIndex, _packet );
	}

	//================================================================================================================================
	// Generate a RPC replication packet for spawning entities
	//================================================================================================================================
	PacketReplication ClientRPC::RPCSpawn( const SpawnInfo& spawnInfo )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << RpcId( 'SPAN' );
		packet.packetData << spawnInfo.spawnFrameIndex;

		sf::Packet dataCpy = spawnInfo.data;
		while( !dataCpy.endOfPacket() )
		{
			uint8_t dataByte;
			dataCpy >> dataByte;
			packet.packetData << dataByte;
		}

		return packet;
	}

	//================================================================================================================================
	// ShiftClientFrame RPC - unwrap data & synchronizes the frame index of the client depending on its rtt
	//================================================================================================================================
	void ClientRPC::UnwrapShiftClientFrame( sf::Packet& _packet )
	{
		sf::Int32 framesDelta;
		_packet >> framesDelta;
		onShiftFrameIndex.Emmit( framesDelta );
	}

	//================================================================================================================================
	// spawns the clients spaceship at a specific frame
	//================================================================================================================================
	PacketReplication ClientRPC::RPCSpawnClientShip( const NetID _spaceshipID, const FrameIndex _frameIndex )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << RpcId( 'SPWN' );
		packet.packetData << _spaceshipID;
		packet.packetData << FrameIndexNet(_frameIndex);

		return packet;
	}

	//================================================================================================================================
	// spawns the clients spaceship at a specific frame
	//================================================================================================================================
	void ClientRPC::UnwrapSpawnClientShip( sf::Packet& _packet )
	{
		FrameIndexNet frameIndex;
		NetID spaceshipID;

		_packet >> spaceshipID;
		_packet >> frameIndex;
		onSpawnClientShip.Emmit( spaceshipID, frameIndex );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRPC::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientRPC& rpc = static_cast<ClientRPC&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "rpc list: " );
			ImGui::Indent();
			for ( std::pair<RpcId, RpcUnwrapFunc> pair : rpc.nameToRPCTable )
			{
				char* charArray = (char*)(&pair.first);
				std::stringstream ss;
				ss << charArray[3] << charArray[2] << charArray[1] << charArray[0];
				ImGui::Text( ss.str().c_str() );
			}
			ImGui::Unindent();
		}ImGui::Unindent(); ImGui::Unindent();
	}
}