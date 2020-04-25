#include "network/singletonComponents/fanLinkingContext.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( LinkingContext );

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &LinkingContext::Init;
		_info.onGui = &LinkingContext::OnGui;
		_info.name = "linking context";
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );
		linkingContext.netIDToEntityHandle.clear();
		linkingContext.entityHandleToNetID.clear();
		linkingContext.nextNetID = 1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::AddEntity( const EntityHandle _entityHandle, const NetID _netID )
	{
		assert( entityHandleToNetID.find( _entityHandle ) == entityHandleToNetID.end() );
		assert( netIDToEntityHandle.find( _netID ) == netIDToEntityHandle.end() );		
		netIDToEntityHandle[_netID] = _entityHandle;
		entityHandleToNetID[_entityHandle] = _netID;
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::RemoveEntity( const EntityHandle _entityHandle )
	{
		const NetID netID = entityHandleToNetID[ _entityHandle ];
		netIDToEntityHandle.erase( netID );
		entityHandleToNetID.erase( _entityHandle );
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContext::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "NetID <-> EntityHandle" );
			for ( std::pair<NetID, EntityHandle> pair : linkingContext.netIDToEntityHandle )
			{
				ImGui::Text( "%d <-> %d", pair.first, pair .second );
			}
		}ImGui::Unindent(); ImGui::Unindent();
	}
}