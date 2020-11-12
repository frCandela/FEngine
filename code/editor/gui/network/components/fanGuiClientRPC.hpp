#pragma once

#include "network/components/fanClientRPC.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientRPC::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &ClientRPC::OnGui;
		_info.mName  = "Client RPC";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientRPC::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ClientRPC& rpc = static_cast<ClientRPC&>( _component );
		ImGui::Text( "rpc list: " );
		ImGui::Indent();
		for( std::pair<RpcID, RpcUnwrapMethod> pair : rpc.mNameToRPCTable )
		{
			ImGui::Text( "%d", pair.first );
		}
		ImGui::Unindent();
	}
}