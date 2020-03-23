#include "editor/singletonComponents/fanEditorGrid.hpp"

#include "core/fanSerializedValues.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( EditorGrid );

	//================================================================================================================================
	//================================================================================================================================
	void EditorGrid::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NONE;
		_info.init = &EditorGrid::Init;
		_info.name = "editor grid";
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGrid::Init( SingletonComponent& _component )
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
	void EditorGrid::Draw( const EditorGrid& _grid )
	{
		if( _grid.isVisible == true )
		{
			const float size = _grid.spacing;
			const int count = _grid.linesCount;

			for( int coord = -_grid.linesCount; coord <= _grid.linesCount; coord++ )
			{
				RendererDebug::Get().DebugLine( _grid.offset + btVector3( -count * size, 0.f, coord * size ), _grid.offset + btVector3( count * size, 0.f, coord * size ), _grid.color );
				RendererDebug::Get().DebugLine( _grid.offset + btVector3( coord * size, 0.f, -count * size ), _grid.offset + btVector3( coord * size, 0.f, count * size ), _grid.color );
			}
		}
	}
}