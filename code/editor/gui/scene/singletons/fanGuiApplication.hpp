#prama once

#include "scene/singletons/fanApplication.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Application::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Application16;
		_info.mGroup      = EngineGroups::Scene;
		_info.mName       = "application";
	}
}
