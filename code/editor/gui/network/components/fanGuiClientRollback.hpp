#include "network/components/fanClientRollback.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientRollback::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::Network16;
		_info.mGroup      = EngineGroups::Network;
		_info.onGui       = &ClientRollback::OnGui;
		_info.save        = &ClientRollback::Save;
		_info.load        = &ClientRollback::Load;
		_info.mName       = "client rollback";
		_info.mEditorPath = "network/";
	}

	//========================================================================================================
	//========================================================================================================
	void ClientRollback::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );
		clientRollback.mRollbackDatas.clear();
	}

	//========================================================================================================
	//========================================================================================================
	void ClientRollback::Save( const EcsComponent& /*_component*/, Json& /*_json*/ ) {}
	void ClientRollback::Load( EcsComponent& /*_component*/, const Json& /*_json*/ ) {}

	//========================================================================================================
	//========================================================================================================
	void ClientRollback::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ImGui::Indent(); ImGui::Indent();
		{
			ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );

            const FrameIndex newestFrameIndex = clientRollback.mRollbackDatas.empty()
                                                ? 0
                                                : clientRollback.mRollbackDatas.back().mFrameIndex;
            const FrameIndex oldestFrameIndex = clientRollback.mRollbackDatas.empty()
                                                ? 0
                                                : clientRollback.mRollbackDatas.front().mFrameIndex;
			
			ImGui::Text( "Num saved states: %d", clientRollback.mRollbackDatas.size() );
			ImGui::Text( "Range [%d,%d]", oldestFrameIndex, newestFrameIndex );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}