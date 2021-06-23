#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/fanColor.hpp"
#include "core/math/fanVector3.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // The grid in the 3D view of the editor
    //==================================================================================================================================================================================================
    struct EditorGrid : public EcsSingleton
    {
    ECS_SINGLETON( EditorGrid )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        Vector3 mOffset;
        Color   mColor;
        Fixed   mSpacing;
        int     mLinesCount;
        bool    mIsVisible;

        static void Draw( EcsWorld& _world );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorGrid
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor grid";
            info.mIcon       = ImGui::Grid16;
            info.mGroup      = EngineGroups::Editor;
            info.onGui       = GuiEditorGrid::OnGui;
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            EditorGrid& grid = static_cast<EditorGrid&>( _component );
            ImGui::MenuItem( "visible", nullptr, &grid.mIsVisible );
            ImGui::DragFixed( "spacing", &grid.mSpacing, 0.25f, 0.f, 100.f );
            ImGui::DragInt( "lines count", &grid.mLinesCount, 1.f, 0, 1000 );
            ImGui::ColorEdit4( "color", &grid.mColor[0], ImGui::fanColorEditFlags );

            glm::vec3 offset = grid.mOffset.ToGlm();
            if( ImGui::DragFloat3( "offset", &offset.x ) ){ grid.mOffset = Vector3( offset ); }
        }
    };
}