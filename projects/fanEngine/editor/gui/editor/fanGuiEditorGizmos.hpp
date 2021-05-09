#pragma once

#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorGizmos
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor gizmo";
            info.mIcon       = ImGui::Gizmos16;
            info.mGroup      = EngineGroups::Editor;
            return info;
        }
    };
}