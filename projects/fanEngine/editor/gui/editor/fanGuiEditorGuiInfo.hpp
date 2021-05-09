#pragma once

#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorGuiInfo
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor gui info";
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Editor;
            info.onGui       = &GuiEditorGuiInfo::OnGui;
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _singleton )
        {
            EditorGuiInfo& gui = static_cast<EditorGuiInfo&>( _singleton );
            (void)gui;
        }
    };
}