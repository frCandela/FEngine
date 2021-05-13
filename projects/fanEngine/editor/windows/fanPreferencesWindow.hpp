#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/input/fanInputManager.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct EditorSettings;

    //==================================================================================================================================================================================================
    // shortcut & axis edition
    // imgui colors
    //==================================================================================================================================================================================================
    struct PreferencesWindow : EcsSingleton
    {
    ECS_SINGLETON( PreferencesWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        static void LogColorsCppInitCode();
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiPreferencesWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "preferences";
            info.mIcon       = ImGui::Preferences16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiPreferencesWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
        static void DrawJoysticks( PreferencesWindow& _preferencesWindow );
        static void DrawShortcuts( EditorSettings& _editorSettings );
    };
}
