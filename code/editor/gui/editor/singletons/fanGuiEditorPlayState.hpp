#pragma once

#include "editor/singletons/fanEditorPlayState.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiEditorPlayState
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName  = "editor play state";
            info.mIcon  = ImGui::IconType::Play16;
            info.mGroup = EngineGroups::Editor;
            return info;
        }
    };
}