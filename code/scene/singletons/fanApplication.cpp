#include "core/fanDebug.hpp"
#include "fanApplication.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Application::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Application16;
		_info.mGroup      = EngineGroups::Scene;
		_info.mName       = "application";
        _info.mSlots.push_back( new Slot<>("quit", &Application::OnQuit ) );
	}

	//========================================================================================================
	//========================================================================================================
	void Application::Init( EcsWorld& /*_world*/, EcsSingleton& _this )
	{
        Application& mainMenu = static_cast<Application&>( _this );
        (void)mainMenu;
	}

    //========================================================================================================
    //========================================================================================================
    void Application::OnQuit( EcsSingleton& _this )
    {
        Application& app = static_cast<Application&>( _this );
        app.mOnQuit.Emmit();
    }
}
