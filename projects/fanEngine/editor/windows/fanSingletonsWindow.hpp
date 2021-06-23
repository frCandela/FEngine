#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // shows the singletons from the game ecs
    //==================================================================================================================================================================================================
    struct SingletonsWindow : EcsSingleton
    {
    ECS_SINGLETON( SingletonsWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSingletonsWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "singletons";
            info.mIcon       = ImGui::Singleton16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiSingletonsWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}