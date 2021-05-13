#pragma once

#include "glfw/glfw3.h"
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/ecs/fanSignal.hpp"
#include "engine/game/fanLaunchSettings.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // 3D view displaying a world in a game
    //==================================================================================================================================================================================================
    struct GameViewWindow : EcsSingleton
    {
    ECS_SINGLETON( GameViewWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        Signal<VkExtent2D> mOnSizeChanged;
        Signal<>           mOnPlay;
        Signal<>           mOnPause;
        Signal<>           mOnResume;
        Signal<>           mOnStop;
        Signal<>           mOnStep;
        Signal<int>        mOnSelectGame;

        glm::vec2 mSize        = glm::vec2( 1.f, 1.f );
        glm::vec2 mPosition;
        bool      mIsHovered;
        char      mStringGameSelectionCombo[16];
        int       mCurrentGame = 0;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiGameViewWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName       = "game view";
            info.mIcon             = ImGui::Joystick16;
            info.mGroup            = EngineGroups::Editor;
            info.mType             = GuiSingletonInfo::Type::ToolWindow;
            info.mImGuiWindowFlags = ImGuiWindowFlags_MenuBar;
            info.onGui             = &GuiGameViewWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}