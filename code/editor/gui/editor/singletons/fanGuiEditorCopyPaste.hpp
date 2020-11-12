#pragma once

#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiEditorCopyPaste
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName  = "editor copy/paste";
            info.mIcon  = ImGui::CopyPaste16;
            info.mGroup = EngineGroups::Editor;
            return info;
        }
    };
} 