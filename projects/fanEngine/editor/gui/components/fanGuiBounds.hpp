#pragma once

#include "engine/components/fanBounds.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiBounds
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mGroup      = EngineGroups::Scene;
            info.mEditorName = "bounds";
            return info;
        }
    };
}