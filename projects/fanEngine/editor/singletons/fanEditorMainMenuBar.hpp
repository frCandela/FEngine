#pragma once

#include <set>
#include "ecs/fanEcsSingleton.hpp"
#include "core/fanSignal.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    struct Scene;

    //==================================================================================================================================================================================================
    // top main menu bar of the editor
    //==================================================================================================================================================================================================
    struct EditorMainMenuBar : EcsSingleton
    {
    ECS_SINGLETON( EditorMainMenuBar )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        static void Open( EcsWorld& _world );
        static void New( EcsWorld& _world );
        static void Reload( EcsWorld& _world );
        static void Save( EcsWorld& _world );
        static void SaveAs( EcsWorld& _world );

        Signal<> mOnReloadShaders;
        Signal<> mOnReloadIcons;
        Signal<> mOnExit;

        bool mShowImguiDemoWindow;
        bool mShowHull;
        bool mShowAABB;
        bool mShowBoundingSphere;
        bool mShowWireframe;
        bool mShowNormals;
        bool mShowLights;
        bool mShowUiBounds;
        bool mShowSkeletons;

        bool mOpenNewScenePopupLater;
        bool mOpenLoadScenePopupLater;
        bool mOpenSaveScenePopupLater;

        // Temporary buffers
        std::string mPathBuffer;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorMainMenuBar
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "main menu bar";
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Editor;
            info.onGui       = &GuiEditorMainMenuBar::OnGui;
            info.mType       = GuiSingletonInfo::Type::Other;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
        static void DrawModals( EditorMainMenuBar& _mainMenuBar, Scene& _scene );
    };
}