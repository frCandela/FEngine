#include "network/components/fanClientRollback.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.group = EngineGroups::Network;
		_info.onGui = &ClientRollback::OnGui;
		_info.save = &ClientRollback::Save;
		_info.load = &ClientRollback::Load;
		_info.name = "client rollback";
		_info.editorPath = "network/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );
		clientRollback.rollbackDatas.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::Save( const EcsComponent& /*_component*/, Json& /*_json*/ ) {}
	void ClientRollback::Load( EcsComponent& /*_component*/, const Json& /*_json*/ ) {}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );

			const FrameIndex newestFrameIndex = clientRollback.rollbackDatas.empty() ? 0 : clientRollback.rollbackDatas.back().frameIndex;
			const FrameIndex oldestFrameIndex = clientRollback.rollbackDatas.empty() ? 0 : clientRollback.rollbackDatas.front().frameIndex;
			
			ImGui::Text( "Num saved states: %d", clientRollback.rollbackDatas.size() );
			ImGui::Text( "Range [%d,%d]", oldestFrameIndex, newestFrameIndex );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}