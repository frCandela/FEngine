#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	struct EditorGrid
	{
		bool		isVisible = true;
		Color		color = Color( 0.161f, 0.290f, 0.8f, 0.478f );
		int			linesCount = 10;
		float		spacing = 1.f;
		btVector3	offset = btVector3::Zero();
	};
}