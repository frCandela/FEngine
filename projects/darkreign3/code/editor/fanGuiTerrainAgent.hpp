#pragma once

#include "game/components/fanTerrainAgent.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTerrainAgent
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::None16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiTerrainAgent::OnGui;
            info.mEditorName = "terrain agent";
            info.mEditorPath = "game/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            TerrainAgent& agent = static_cast<TerrainAgent&>( _component );
            ImGui::DragFixed( "height offset", &agent.mHeightOffset, 0.05f, -2, 20 );
        }
    };
}