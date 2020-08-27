#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void LinkingContext::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::LINK16;
		_info.group = EngineGroups::Network;
		_info.onGui = &LinkingContext::OnGui;
		_info.name = "linking context";
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContext::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );
		linkingContext.mNetIDToEcsHandle.clear();
		linkingContext.mEcsHandleToNetID.clear();
		linkingContext.mNextNetID = 1;
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContext::AddEntity( const EcsHandle _handle, const NetID _netID )
	{
		fanAssert( _handle != 0 );
		fanAssert( _netID != 0 );
		fanAssert( mEcsHandleToNetID.find( _handle ) == mEcsHandleToNetID.end() );
		fanAssert( mNetIDToEcsHandle.find( _netID ) == mNetIDToEcsHandle.end() );
        mNetIDToEcsHandle[_netID]  = _handle;
        mEcsHandleToNetID[_handle] = _netID;
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContext::RemoveEntity( const EcsHandle _handle )
	{
		auto it = mEcsHandleToNetID.find( _handle );
		if( it != mEcsHandleToNetID.end() )
		{
			const NetID netID = it->second;
			mNetIDToEcsHandle.erase( netID );
			mEcsHandleToNetID.erase( _handle );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContext::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );
		ImGui::Columns( 2 );

		ImGui::Text( "net ID" );		ImGui::NextColumn();
		ImGui::Text( "entity handle" ); ImGui::NextColumn();
		for( std::pair<NetID, EcsHandle> pair : linkingContext.mNetIDToEcsHandle )
		{
			ImGui::Text( "%d", pair.first );
			ImGui::NextColumn();
			ImGui::Text( "%d", pair.second );
			ImGui::NextColumn();
		}
		ImGui::Columns( 1 );
	}
}