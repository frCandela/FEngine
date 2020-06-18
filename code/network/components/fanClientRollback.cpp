#include "network/components/fanClientRollback.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
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
		clientRollback.previousStates.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::Save( const EcsComponent& /*_component*/, Json& /*_json*/ ) {}
	void ClientRollback::Load( EcsComponent& /*_component*/, const Json& /*_json*/ ) {}

	//================================================================================================================================
	// @todo, ensure that rollback states indices are contiguous to allow constant access complexity
	//================================================================================================================================
	const ClientRollback::RollbackState* ClientRollback::GetState( const FrameIndex _frameIndex ) const
	{
		// if frame index is in range
		if( _frameIndex <= GetNewestFrameIndex() && _frameIndex >= GetOldestFrameIndex() )
		{
			for( const RollbackState& state : previousStates )
			{
				if( state.frameIndex == _frameIndex )
				{
					return &state;
				}
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientRollback::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );
			ImGui::Text( "Num saved states: %d", clientRollback.previousStates.size() );
			ImGui::Text( "Range [%d,%d]", clientRollback.GetOldestFrameIndex(), clientRollback.GetNewestFrameIndex() );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}