#include "network/singletonComponents/fanLinkingContext.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &LinkingContext::OnGui;
		_info.name = "linking context";
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );
		linkingContext.netIDToEcsHandle.clear();
		linkingContext.EcsHandleToNetID.clear();
		linkingContext.nextNetID = 1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::AddEntity( const EcsHandle _handle, const NetID _netID )
	{
		assert( EcsHandleToNetID.find( _handle ) == EcsHandleToNetID.end() );
		assert( netIDToEcsHandle.find( _netID ) == netIDToEcsHandle.end() );		
		netIDToEcsHandle[_netID] = _handle;
		EcsHandleToNetID[_handle] = _netID;
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::RemoveEntity( const EcsHandle _handle )
	{
		const NetID netID = EcsHandleToNetID[ _handle ];
		netIDToEcsHandle.erase( netID );
		EcsHandleToNetID.erase( _handle );
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );

		ImGui::Indent(); ImGui::Indent();
		ImGui::Columns( 2 );

		ImGui::Text( "net ID" );		ImGui::NextColumn();
		ImGui::Text( "entity handle" ); ImGui::NextColumn();
		for( std::pair<NetID, EcsHandle> pair : linkingContext.netIDToEcsHandle )
		{
			ImGui::Text( "%d", pair.first );
			ImGui::NextColumn();
			ImGui::Text( "%d", pair.second );
			ImGui::NextColumn();
		}

		ImGui::Columns( 1 );
		ImGui::Unindent(); ImGui::Unindent();
	}
}