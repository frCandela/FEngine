#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void LinkingContextUnregisterer::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.group = EngineGroups::Network;
		_info.onGui = &LinkingContextUnregisterer::OnGui;
		_info.destroy = &LinkingContextUnregisterer::Destroy;
		_info.name = "linking context unregisterer";
	}

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContextUnregisterer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& /*_component*/ )
	{
	}	

	//================================================================================================================================
	//================================================================================================================================
	void LinkingContextUnregisterer::Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& /*_component*/ )
	{
		LinkingContext& linkingContext =_world.GetSingleton<LinkingContext>();
		linkingContext.RemoveEntity( _world.GetHandle( _entity ) );
	}
	//================================================================================================================================
	//================================================================================================================================
	void LinkingContextUnregisterer::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& /*_component*/ )
	{
// 		LinkingContextUnregisterer& linkingContextUnregisterer = static_cast<LinkingContextUnregisterer&>( _component );
// 		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
// 		{
// 		} ImGui::PopItemWidth();
	}
}