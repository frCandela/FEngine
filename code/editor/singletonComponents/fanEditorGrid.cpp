#include "editor/singletonComponents/fanEditorGrid.hpp"

#include "core/fanSerializedValues.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( EditorGrid, "editor_grid" );

	//================================================================================================================================
	//================================================================================================================================
	EditorGrid::EditorGrid() :
		  isVisible ( true )
		, color		( Color( 0.161f, 0.290f, 0.8f, 0.478f ) )
		, linesCount( 10 )
		, spacing	( 1.f )
		, offset	( btVector3::Zero() )
	{
		SerializedValues::Get().GetBool( "editor_grid_show", isVisible );
		SerializedValues::Get().GetColor( "editor_grid_color", color );
		SerializedValues::Get().GetInt( "editor_grid_linesCount", linesCount );
		SerializedValues::Get().GetFloat( "editor_grid_spacing", spacing );
		SerializedValues::Get().GetVec3( "editor_grid_offset", offset );
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