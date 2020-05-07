#include "network/singletonComponents/fanRPCManager.hpp"

#include <sstream>
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( RPCManager );

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &RPCManager::Init;
		_info.onGui = &RPCManager::OnGui;
		_info.name = "RPC manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		RPCManager& rpc = static_cast<RPCManager&>( _component );
		rpc.nameToRPCTable.clear();
		rpc.onShiftFrameIndex.Clear();
		rpc.onSpawnShip.Clear();

		rpc.RegisterRPCs();
	}

	//================================================================================================================================
	// Registers all available RPC unwrap functions
	//================================================================================================================================
	void RPCManager::RegisterRPCs( )
	{
		RegisterUnwrapFunction( 'SYNC', &RPCManager::UnwrapShiftClientFrame );
		RegisterUnwrapFunction( 'SPWN', &RPCManager::UnwrapSpawnShip );
	}

	//================================================================================================================================
	// Registers a RPC unwrap function
	// A unwrap function must decode a packet and run the corresponding procedure
	//================================================================================================================================
	void RPCManager::RegisterUnwrapFunction( const RpcId _id, const RpcUnwrapFunc _rpcUnwrapFunc )
	{
		assert( nameToRPCTable.find( _id ) == nameToRPCTable.end() );
		nameToRPCTable[_id] = _rpcUnwrapFunc;
	}

	//================================================================================================================================
	// Runs a procedure from an incoming rpc packet data
	//================================================================================================================================
	void RPCManager::TriggerRPC( sf::Packet& _packet )
	{
		RpcId rpcID;
		_packet >> rpcID;

		const RpcUnwrapFunc function =  nameToRPCTable[rpcID];
		( ( *this ).*( function ) )( _packet );
	}

	//================================================================================================================================
	// SynchClientFrame RPC - chan
	//================================================================================================================================
	PacketReplication RPCManager::RPCShiftClientFrame( const int _framesDelta )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << RpcId( 'SYNC' );
		packet.packetData << sf::Int32(_framesDelta);

		return packet;
	}

	//================================================================================================================================
	// SynchClientFrame RPC - unwrap data & synchronizes the frame index of the client depending on its rtt
	//================================================================================================================================
	void RPCManager::UnwrapShiftClientFrame( sf::Packet& _packet )
	{
		sf::Int32 framesDelta;
		_packet >> framesDelta;
		onShiftFrameIndex.Emmit( framesDelta );
	}

	//================================================================================================================================
	//================================================================================================================================
	PacketReplication RPCManager::RPCSSpawnShip( const NetID _spaceshipID, const FrameIndex _frameIndex )
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
	//================================================================================================================================
	void RPCManager::UnwrapSpawnShip( sf::Packet& _packet )
	{
		FrameIndexNet frameIndex;
		NetID spaceshipID;

		_packet >> spaceshipID;
		_packet >> frameIndex;
		onSpawnShip.Emmit( spaceshipID, frameIndex );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		RPCManager& rpc = static_cast<RPCManager&>( _component );

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