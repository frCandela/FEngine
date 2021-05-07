#pragma once

#include "core/ecs/fanSignal.hpp"
#include "engine/project/fanLaunchSettings.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
    struct NoiseOctave;
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class TerrainWindow : public EditorWindow
    {
    public:
        TerrainWindow();

    private:
        void OnGui( EcsWorld& _world ) override;
        static bool GuiNoiseOctave( const char* _name, NoiseOctave& _octave );
    };
}