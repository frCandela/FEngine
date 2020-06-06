#pragma once

#include <unordered_map>
#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"
#include "ecs/fanSignal.hpp"

namespace fan
{
	//================================================================================================================================
	// Manages remote procedure calls
	// A remote procedure call is the act of one host causing a	procedure to execute on one or more remote hosts
	// Can generate replication packets to send using the server replication
	//================================================================================================================================	
	struct ClientRPC : public EcsComponent
	{
		ECS_COMPONENT( ClientRPC )
	public:
		//================================================================
		//================================================================
		struct BulletSpawnInfo
		{
			NetID			owner;
			btVector3		position = btVector3::Zero();
			btVector3		velocity = btVector3::Zero();
			FrameIndexNet	spawnFrameIndex;
		};

		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		using RpcUnwrapFunc = void ( ClientRPC::* )( sf::Packet& );
		using RpcId = sf::Uint32;

		void RegisterRPCs( );
		void RegisterUnwrapFunction( const RpcId _id, const RpcUnwrapFunc _rpcUnwrapFunc );
		void TriggerRPC( sf::Packet& _packet );

		std::unordered_map<RpcId , RpcUnwrapFunc > nameToRPCTable;

		// List of available rpc
		Signal < int >			 onShiftFrameIndex;
		void					 UnwrapShiftClientFrame( sf::Packet& _packet );
		static PacketReplication RPCShiftClientFrame( const int _framesDelta );

		Signal < NetID, FrameIndex > onSpawnClientShip;
		void					 UnwrapSpawnClientShip( sf::Packet& _packet );
		static PacketReplication RPCSpawnClientShip( const NetID _spaceshipID, const FrameIndex _frameIndex );

		Signal < NetID, FrameIndex > onSpawnShip;
		void					 UnwrapSpawnShip( sf::Packet& _packet);
		static PacketReplication RPCSpawnShip( const NetID _spaceshipID, const FrameIndex _frameIndex );

		Signal < BulletSpawnInfo > onSpawnBullet;
		void					 UnwrapSpawnBullet( sf::Packet& _packet );
		static PacketReplication RPCSpawnBullet( const BulletSpawnInfo& _spawnInfo );
	};
}