#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    class Renderer;

    //==================================================================================================================================================================================================
    // shows data from the renderer
    // loaded mesh, textures, prefabs, lights & buffer sizes
    //==================================================================================================================================================================================================
    struct RenderWindow : EcsSingleton
    {
    ECS_SINGLETON( RenderWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        Renderer* mRenderer;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiRenderWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "renderer";
            info.mIcon       = ImGui::Renderer16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiRenderWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}