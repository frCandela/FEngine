#pragma once

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct GuiSingletonInfo
    {
        std::string            mName;
        ImGui::IconType        mIcon  = ImGui::IconType::None16;
        EngineGroups           mGroup = EngineGroups::None;

        void ( *onGui ) ( EcsWorld&, EcsSingleton& ) = nullptr;
    };

    //========================================================================================================
    //========================================================================================================
    struct GuiComponentInfo
    {
        std::string            mName;
        ImGui::IconType        mIcon  = ImGui::IconType::None16;	// editor icon
        EngineGroups           mGroup = EngineGroups::None;	    // editor group
        const char*		       mEditorPath = "";				// editor path for the addComponent menu

        void ( *onGui )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			// draw gui
    };
}


