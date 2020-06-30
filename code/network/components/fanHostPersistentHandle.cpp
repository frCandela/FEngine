#include "network/components/fanHostPersistentHandle.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void HostPersistentHandle::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.group = EngineGroups::Network;
		_info.onGui = &HostPersistentHandle::OnGui;
		_info.name = "network handle";
		_info.editorPath = "network/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostPersistentHandle::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostPersistentHandle& networkHandle = static_cast<HostPersistentHandle&>( _component );
		networkHandle.handle = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostPersistentHandle::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			HostPersistentHandle& networkHandle = static_cast<HostPersistentHandle&>( _component );
			ImGui::Text( "network handle : %d", networkHandle.handle );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}