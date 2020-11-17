#pragma once

#include "editor/singletons/fanEditorGrid.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiEditorGrid
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName  = "editor grid";
            info.mIcon  = ImGui::Grid16;
            info.mGroup = EngineGroups::Editor;
            info.onGui  = GuiEditorGrid::OnGui;
            return info;
        }

        //====================================================================================================
        //====================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            EditorGrid& grid = static_cast<EditorGrid&>( _component );
            ImGui::MenuItem( "visible", nullptr, &grid.mIsVisible );
            ImGui::DragFloat( "spacing", &grid.mSpacing, 0.25f, 0.f, 100.f );
            ImGui::DragInt( "lines count", &grid.mLinesCount, 1.f, 0, 1000 );
            ImGui::ColorEdit4( "color", &grid.mColor[0], ImGui::fanColorEditFlags );
            ImGui::DragFloat3( "offset", &grid.mOffset[0] );
        }
    };
}