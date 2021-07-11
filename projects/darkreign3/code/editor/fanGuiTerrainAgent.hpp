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
            info.mIcon       = ImGui::IconType::Terrain16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiTerrainAgent::OnGui;
            info.mEditorName = "Terrain agent";
            info.mEditorPath = "game/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            TerrainAgent& agent = static_cast<TerrainAgent&>( _component );
            ImGui::DragFixed( "height offset", &agent.mHeightOffset, 0.05f, -2, 20 );
            ImGui::DragFixed( "move speed", &agent.mMoveSpeed, 0.1f, 0, 100 );
            ImGui::DragFixed( "rotation speed", &agent.mRotationSpeed, 0.1f, 0, 720 );
            ImGui::DragFixed( "range", &agent.mRange );
            ImGui::Checkbox( "align with terrain", &agent.mAlignWithTerrain );
            ImGui::PushReadOnly();
            ImGui::DragFixed3( "target position", &agent.mDestination.x );
            Fixed distanceFromDestination = Fixed::Sqrt( agent.mSqrDistanceFromDestination );
            ImGui::DragFixed( "distance from destination", &distanceFromDestination );
            switch( agent.mState )
            {
                case TerrainAgent::Stay:
                    ImGui::Text( "<stay>" );
                    break;
                case TerrainAgent::Move:
                    ImGui::Text( "<move>" );
                    break;
                case TerrainAgent::Face:
                    ImGui::Text( "<face>" );
                    break;
                default:
                    fanAssert( false );
                    break;
            }
            ImGui::PopReadOnly();
        }
    };
}