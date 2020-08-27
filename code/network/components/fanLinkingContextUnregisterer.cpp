#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void LinkingContextUnregisterer::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.group = EngineGroups::Network;
		_info.onGui = &LinkingContextUnregisterer::OnGui;
		_info.destroy = &LinkingContextUnregisterer::Destroy;
		_info.name = "linking context unregisterer";
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContextUnregisterer::Init( EcsWorld& , EcsEntity /*_entity*/, EcsComponent& /*_cpnt*/ )
	{
	}	

	//========================================================================================================
	//========================================================================================================
	void LinkingContextUnregisterer::Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& /*_cpnt*/ )
	{
		LinkingContext& linkingContext =_world.GetSingleton<LinkingContext>();
		linkingContext.RemoveEntity( _world.GetHandle( _entity ) );
	}
	//========================================================================================================
	//========================================================================================================
	void LinkingContextUnregisterer::OnGui( EcsWorld& , EcsEntity /*_entity*/, EcsComponent& /*_cpnt*/ )
	{
	}
}