#include "editor/singletons/fanEditorGrid.hpp"

#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorGrid::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::GRID16;
		_info.mName  = "editor grid";
		_info.mGroup = EngineGroups::Editor;
        _info.save   = EditorGrid::Save;
        _info.load   = EditorGrid::Load;
        _info.onGui  = EditorGrid::OnGui;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorGrid::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		EditorGrid& editorGrid = static_cast<EditorGrid&>( _component );
		editorGrid.mOffset     = btVector3::Zero();
		editorGrid.mColor      = Color( 0.161f, 0.290f, 0.8f, 0.478f );
		editorGrid.mSpacing    = 1.f;
		editorGrid.mLinesCount = 10;
		editorGrid.mIsVisible  = true;
	}

    //========================================================================================================
    //========================================================================================================
    void EditorGrid::Save( const EcsSingleton& _component, Json& _json )
    {
        const EditorGrid& grid = static_cast< const EditorGrid&>( _component );
        Serializable::SaveVec3(  _json, "offset",  grid.mOffset );
        Serializable::SaveColor( _json, "color",   grid.mColor );
        Serializable::SaveFloat( _json, "spacing", grid.mSpacing );
        Serializable::SaveInt(   _json, "count",   grid.mLinesCount );
        Serializable::SaveBool(  _json, "visible", grid.mIsVisible );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorGrid::Load( EcsSingleton& _component, const Json& _json )
    {
        EditorGrid& grid = static_cast<EditorGrid&>( _component );
        Serializable::LoadVec3(  _json, "offset",  grid.mOffset );
        Serializable::LoadColor( _json, "color",   grid.mColor );
        Serializable::LoadFloat( _json, "spacing", grid.mSpacing );
        Serializable::LoadInt(   _json, "count",   grid.mLinesCount );
        Serializable::LoadBool(  _json, "visible", grid.mIsVisible );
    }

	//========================================================================================================
	//========================================================================================================
	void EditorGrid::Draw( EcsWorld& _world )
	{
		EditorGrid& grid = _world.GetSingleton<EditorGrid>();
		if( grid.mIsVisible == true )
		{
			const float size = grid.mSpacing;
			const int count = grid.mLinesCount;

			for( int coord = -grid.mLinesCount; coord <= grid.mLinesCount; coord++ )
			{
				_world.GetSingleton<RenderDebug>().DebugLine( grid.mOffset +
                                                              btVector3( -count * size, 0.f, coord * size ),
                                                              grid.mOffset +
                                                              btVector3( count * size, 0.f, coord * size ),
                                                              grid.mColor );

				_world.GetSingleton<RenderDebug>().DebugLine( grid.mOffset +
                                                              btVector3( coord * size, 0.f, -count * size ),
                                                              grid.mOffset +
                                                              btVector3( coord * size, 0.f, count * size ),
                                                              grid.mColor );
            }
		}
	}

    //========================================================================================================
    //========================================================================================================
    void EditorGrid::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        EditorGrid& grid = static_cast<EditorGrid&>( _component );
        ImGui::MenuItem( "visible", nullptr, &grid.mIsVisible );
        ImGui::DragFloat( "spacing", &grid.mSpacing, 0.25f, 0.f, 100.f );
        ImGui::DragInt( "lines count", &grid.mLinesCount, 1.f, 0, 1000 );
        ImGui::ColorEdit3( "color", &grid.mColor[0], ImGui::fanColorEditFlags );
        ImGui::DragFloat3( "offset", &grid.mOffset[0] );
    }
}