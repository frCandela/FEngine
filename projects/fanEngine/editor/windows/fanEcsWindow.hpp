#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // displays the state of the ecs world ( entities, components, memory consumption etc. )
    //==================================================================================================================================================================================================
    struct EcsWindow : EcsSingleton
    {
    ECS_SINGLETON( EcsWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEcsWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "ecs";
            info.mIcon       = ImGui::Ecs16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiEcsWindow::OnGui;
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}