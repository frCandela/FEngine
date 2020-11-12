#pragma once

#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiLinkingContextUnregisterer
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiLinkingContextUnregisterer::OnGui;
            info.mEditorName = "linking context unregisterer";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsEntity /*_entity*/, EcsComponent& /*_cpnt*/ )
        {
        }
    };
}