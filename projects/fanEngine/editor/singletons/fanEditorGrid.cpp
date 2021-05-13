#include "editor/singletons/fanEditorGrid.hpp"

#include "core/ecs/fanEcsWorld.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGrid::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = EditorGrid::Save;
        _info.load = EditorGrid::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGrid::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        EditorGrid& editorGrid = static_cast<EditorGrid&>( _component );
        editorGrid.mOffset     = Vector3::sZero;
        editorGrid.mColor      = Color::sGrey;
        editorGrid.mSpacing    = 1;
        editorGrid.mLinesCount = 10;
        editorGrid.mIsVisible  = true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGrid::Save( const EcsSingleton& _component, Json& _json )
    {
        const EditorGrid& grid = static_cast< const EditorGrid&>( _component );
        Serializable::SaveVec3( _json, "offset", grid.mOffset );
        Serializable::SaveColor( _json, "color", grid.mColor );
        Serializable::SaveFixed( _json, "spacing", grid.mSpacing );
        Serializable::SaveInt( _json, "count", grid.mLinesCount );
        Serializable::SaveBool( _json, "visible", grid.mIsVisible );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGrid::Load( EcsSingleton& _component, const Json& _json )
    {
        EditorGrid& grid = static_cast<EditorGrid&>( _component );
        Serializable::LoadVec3( _json, "offset", grid.mOffset );
        Serializable::LoadColor( _json, "color", grid.mColor );
        Serializable::LoadFixed( _json, "spacing", grid.mSpacing );
        Serializable::LoadInt( _json, "count", grid.mLinesCount );
        Serializable::LoadBool( _json, "visible", grid.mIsVisible );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGrid::Draw( EcsWorld& _world )
    {
        EditorGrid& grid = _world.GetSingleton<EditorGrid>();
        if( grid.mIsVisible == true )
        {
            const Fixed size  = grid.mSpacing;
            const int   count = grid.mLinesCount;

            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( int coord = -grid.mLinesCount; coord <= grid.mLinesCount; coord++ )
            {
                rd.DebugLine( grid.mOffset + Vector3( -count * size, 0, coord * size ), grid.mOffset + Vector3( count * size, 0, coord * size ), grid.mColor, true );
                rd.DebugLine( grid.mOffset + Vector3( coord * size, 0, -count * size ), grid.mOffset + Vector3( coord * size, 0, count * size ), grid.mColor, true );
            }
        }
    }
}