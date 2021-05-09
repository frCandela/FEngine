#pragma once

#include "engine/singletons/fanApplication.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiApplication
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::IconType::Application16;
            info.mGroup      = EngineGroups::Scene;
            info.mEditorName = "application";
            return info;
        }
    };
}
