#pragma once

#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiEditorSelection
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName  = "editor selection";
            info.mIcon  = ImGui::Selection16;
            info.mGroup = EngineGroups::Editor;
            return info;
        }
    };
}