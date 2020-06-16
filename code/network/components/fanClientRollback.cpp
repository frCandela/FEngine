#include "network/components/fanClientRollback.hpp"

// #include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
// #include "core/time/fanTime.hpp"
// #include "network/fanImGuiNetwork.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &ClientRollback::OnGui;
		_info.name = "client rollback";
		_info.editorPath = "network/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );
		clientRollback.previousStates.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );
			ImGui::Text( "Num saved states: %d", clientRollback.previousStates.size() );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}