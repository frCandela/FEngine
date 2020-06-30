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
		_info.group = EngineGroups::Network;
		_info.onGui = &ClientRPC::OnGui;
		_info.name = "Client RPC";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRPC::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientRPC& rpc = static_cast<ClientRPC&>( _component );
		rpc.nameToRPCTable.clear();
		rpc.onShiftFrameIndex.Clear();
		rpc.onSpawn.Clear();
		rpc.RegisterRPCs();
	}

	//================================================================================================================================
	// register one rpc unwrap method, checks that there is no RpcID collision
	//================================================================================================================================
	void ClientRPC::RegisterUnwrapMethod( const RpcID _rpcID, const RpcUnwrapMethod _unwrapMethod )
	{
		assert( nameToRPCTable.find( _rpcID ) == nameToRPCTable.end() );
		nameToRPCTable[_rpcID] = _unwrapMethod;
	}

	//================================================================================================================================
	// Registers  RPC unwrap functions
	// A unwrap function must decode a packet and run the corresponding procedure
	//================================================================================================================================
	void ClientRPC::RegisterRPCs( )
	{
		nameToRPCTable.clear(); 
		RegisterUnwrapMethod( s_rpcIdShiftFrame, &ClientRPC::UnwrapShiftClientFrame );
		RegisterUnwrapMethod( s_rpcIdSpawn, &ClientRPC::UnwrapSpawn );
	}

	//================================================================================================================================
	// Runs a procedure from an incoming rpc packet data
	//================================================================================================================================
	void ClientRPC::TriggerRPC( sf::Packet& _packet )
	{
		RpcID rpcID;
		_packet >> rpcID;

		const RpcUnwrapMethod method =  nameToRPCTable[rpcID];
		( ( *this ).*( method ) )( _packet );
	}

	//================================================================================================================================
	// SynchClientFrame RPC
	//================================================================================================================================
	PacketReplication ClientRPC::RPCShiftClientFrame( const int _framesDelta )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << s_rpcIdShiftFrame;
		packet.packetData << sf::Int32(_framesDelta);

		return packet;
	}

	//================================================================================================================================
	// Must be connected to the SpawnManager, copy packet data for future spawning
	//================================================================================================================================
	void ClientRPC::UnwrapSpawn( sf::Packet& _packet )
	{
		FrameIndex frameIndex;
		SpawnID spawnID;
		_packet >> spawnID;
		_packet >> frameIndex;
		onSpawn.Emmit( spawnID, frameIndex, _packet );
	}

	//================================================================================================================================
	// Generate a RPC replication packet for spawning entities
	//================================================================================================================================
	PacketReplication ClientRPC::RPCSpawn( const SpawnInfo& spawnInfo )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << s_rpcIdSpawn;
		packet.packetData << spawnInfo.spawnID;
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
	//================================================================================================================================
	void ClientRPC::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientRPC& rpc = static_cast<ClientRPC&>( _component );
		ImGui::Text( "rpc list: " );
		ImGui::Indent();
		for( std::pair<RpcID, RpcUnwrapMethod> pair : rpc.nameToRPCTable )
		{
			ImGui::Text( "%d", pair.first );
		}
		ImGui::Unindent();
	}
}