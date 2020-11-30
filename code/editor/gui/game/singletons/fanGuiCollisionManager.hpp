#pragma once

#include "project_spaceships/singletons/fanCollisionManager.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiCollisionManager
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Rigidbody16;
            info.mGroup = EngineGroups::Game;
            info.mEditorName  = "collision manager";
            return info;
        }
    };
}