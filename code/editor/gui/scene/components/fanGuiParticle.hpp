#pragma once

#include "scene/components/fanParticle.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiParticle
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mEditorName = "particle";
            info.mGroup      = EngineGroups::Scene;
            return info;
        }
    };
}