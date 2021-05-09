#pragma once

#include "engine/terrain/fanVoxelTerrain.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiVoxelTerrain
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Terrain16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiVoxelTerrain::OnGui;
            info.mEditorName = "voxel terrain";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            VoxelTerrain& terrain = static_cast<VoxelTerrain&>( _singleton );

            ImGui::Indent();
            {
                // chunk size
                ImGui::FanShowHelpMarker( "Open the terrain editor to edit the terrain." );
                ImGui::SameLine();
                ImGui::PushReadOnly();
                glm::ivec3 terrainSize = terrain.mSize;
                ImGui::DragInt3( "terrain sizea", &terrainSize.x );
                ImGui::PopReadOnly();
            }
            ImGui::Unindent();
        }
    };
}