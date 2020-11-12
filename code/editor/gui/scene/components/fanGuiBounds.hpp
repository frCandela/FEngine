#pragma once

#include "scene/components/fanBounds.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Bounds::SetInfo( EcsComponentInfo& _info )
	{
		_info.mGroup = EngineGroups::Scene;
		_info.mName  = "bounds";
	}
}