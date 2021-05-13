#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/ecs/fanSignal.hpp"
#include "engine/project/fanLaunchSettings.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct NoiseOctave;
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct TerrainWindow : EcsSingleton
    {
    ECS_SINGLETON( TerrainWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTerrainWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "terrain editor";
            info.mIcon       = ImGui::Terrain16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiTerrainWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
        static bool GuiNoiseOctave( const char* _name, NoiseOctave& _octave );
    };
}