#include "editor/singletons/fanEditorGrid.hpp"

#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void EditorGrid::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::GRID16;
		_info.name = "editor grid";
		_info.group = EngineGroups::Editor;
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGrid::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		EditorGrid& editorGrid = static_cast<EditorGrid&>( _component );
		editorGrid.offset = btVector3::Zero();
		editorGrid.color = Color( 0.161f, 0.290f, 0.8f, 0.478f );
		editorGrid.spacing = 1.f;
		editorGrid.linesCount = 10;
		editorGrid.isVisible = true;

		SerializedValues::Get().GetBool( "editor_grid_show", editorGrid.isVisible );
		SerializedValues::Get().GetColor( "editor_grid_color", editorGrid.color );
		SerializedValues::Get().GetInt( "editor_grid_linesCount", editorGrid.linesCount );
		SerializedValues::Get().GetFloat( "editor_grid_spacing", editorGrid.spacing );
		SerializedValues::Get().GetVec3( "editor_grid_offset", editorGrid.offset );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGrid::Draw( EcsWorld& _world )
	{
		EditorGrid& grid = _world.GetSingleton<EditorGrid>();
		if( grid.isVisible == true )
		{
			const float size = grid.spacing;
			const int count = grid.linesCount;

			for( int coord = -grid.linesCount; coord <= grid.linesCount; coord++ )
			{
				_world.GetSingleton<RenderDebug>().DebugLine( grid.offset + btVector3( -count * size, 0.f, coord * size ), grid.offset + btVector3( count * size, 0.f, coord * size ), grid.color );
				_world.GetSingleton<RenderDebug>().DebugLine( grid.offset + btVector3( coord * size, 0.f, -count * size ), grid.offset + btVector3( coord * size, 0.f, count * size ), grid.color );
			}
		}
	}
}