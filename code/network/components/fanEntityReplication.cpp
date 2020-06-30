#include "network/components/fanEntityReplication.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void EntityReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.group = EngineGroups::Network;
		_info.onGui = &EntityReplication::OnGui;
		_info.name = "replication";
		_info.editorPath = "network/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void EntityReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		EntityReplication& replication = static_cast<EntityReplication&>( _component );
		replication.exclude = 0;
		replication.componentTypes.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void EntityReplication::OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			EntityReplication& replication = static_cast<EntityReplication&>( _component );
			ImGui::Text( "exclude: %u", replication.exclude );
			ImGui::Text( "replicated components: " );
			ImGui::Indent();
			for ( uint32_t type : replication.componentTypes )
			{
				const EcsComponentInfo&  info = _world.GetComponentInfo( type );
				ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( "%s", info.name.c_str() );
			}
			ImGui::Unindent();
		}ImGui::Unindent(); ImGui::Unindent();
	}
}