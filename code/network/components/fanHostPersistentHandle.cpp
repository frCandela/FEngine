#include "network/components/fanHostPersistentHandle.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostPersistentHandle::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::Network16;
		_info.mGroup      = EngineGroups::Network;
		_info.onGui       = &HostPersistentHandle::OnGui;
		_info.mName       = "network handle";
		_info.mEditorPath = "network/";
	}

	//========================================================================================================
	//========================================================================================================
	void HostPersistentHandle::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostPersistentHandle& networkHandle = static_cast<HostPersistentHandle&>( _component );
		networkHandle.mHandle = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void HostPersistentHandle::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			HostPersistentHandle& networkHandle = static_cast<HostPersistentHandle&>( _component );
			ImGui::Text( "network handle : %d", networkHandle.mHandle );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}