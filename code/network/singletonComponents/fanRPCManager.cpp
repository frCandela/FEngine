#include "network/singletonComponents/fanRPCManager.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( RPCManager );

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
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

		rpc.RegisterRPCs();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::RegisterRPCs( )
	{
		RegisterUnwrapFunction( 'SYCF', &RPCManager::UnwrapSynchClientFrame );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::RegisterUnwrapFunction( const uint32_t _id, const RPCUnwrapFunc _RPCUnwrapFunc )
	{
		assert( nameToRPCTable.find( _id ) == nameToRPCTable.end() );
		nameToRPCTable[_id] = _RPCUnwrapFunc;
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::RPCSynchClientFrame( sf::Packet& _packet, const sf::Uint64 _frameIndex, const float _RTT )
	{
		_packet << _frameIndex;
		_packet << _RTT;
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::UnwrapSynchClientFrame( sf::Packet& _packet )
	{
		sf::Uint64 frameIndex;
		float RTT;

		_packet >> frameIndex;
		_packet >> RTT;

		OnSynchClientFrame.Emmit( frameIndex, RTT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
	
		}ImGui::Unindent(); ImGui::Unindent();
	}
}