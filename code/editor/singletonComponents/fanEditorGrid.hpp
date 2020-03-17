#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "ecs/fanSingletonComponent.hpp"

namespace fan
{

	//================================================================================================================================
	// The grid in the 3D view of the editor
	//================================================================================================================================
	struct EditorGrid : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:

		EditorGrid();

		btVector3	offset;
		Color		color;
		float		spacing;
		int			linesCount;
		bool		isVisible;

		static void Draw( const EditorGrid& _grid );
	};
}