#pragma once

#include "engine/singletons/fanScenePointers.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiScenePointers
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Pointer16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiScenePointers::OnGui;
            info.mEditorName = "scene pointers";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
            ImGui::Text( "unresolved component pointers: %d", (int)scenePointers.mUnresolvedComponentPtr.size() );
        }
    };
}