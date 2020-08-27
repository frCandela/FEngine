#include "network/components/fanEntityReplication.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EntityReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::Network16;
		_info.mGroup      = EngineGroups::Network;
		_info.onGui       = &EntityReplication::OnGui;
		_info.mName       = "replication";
		_info.mEditorPath = "network/";
	}

	//========================================================================================================
	//========================================================================================================
	void EntityReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		EntityReplication& replication = static_cast<EntityReplication&>( _component );
		replication.mExclude = 0;
		replication.mComponentTypes.clear();
	}

	//========================================================================================================
	//========================================================================================================
	void EntityReplication::OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			EntityReplication& replication = static_cast<EntityReplication&>( _component );
			ImGui::Text( "exclude: %u", replication.mExclude );
			ImGui::Text( "replicated components: " );
			ImGui::Indent();
			for ( uint32_t type : replication.mComponentTypes )
			{
				const EcsComponentInfo&  info = _world.GetComponentInfo( type );
				ImGui::Icon( info.mIcon, { 16, 16 } ); ImGui::SameLine();
				ImGui::Text( "%s", info.mName.c_str() );
			}
			ImGui::Unindent();
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}