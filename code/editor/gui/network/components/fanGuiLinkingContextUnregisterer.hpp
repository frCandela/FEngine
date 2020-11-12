#pragma once

#include "network/components/fanLinkingContextUnregisterer.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void LinkingContextUnregisterer::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon   = ImGui::IconType::Network16;
		_info.mGroup  = EngineGroups::Network;
		_info.onGui   = &LinkingContextUnregisterer::OnGui;
		_info.mName   = "linking context unregisterer";
	}

	//========================================================================================================
	//========================================================================================================
	void LinkingContextUnregisterer::OnGui( EcsWorld& , EcsEntity /*_entity*/, EcsComponent& /*_cpnt*/ )
	{
	}
}