#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class EditorSelection;

	//================================================================================================================================
	// copy paste editor callbacks
	// @todo make this a singleton component in the editor ecs world
	//================================================================================================================================
	class EditorCopyPaste
	{
	public:
		EditorCopyPaste( EditorSelection& _selection );

		// editor callbacks
		void OnCopy();
		void OnPaste();

	private:
		EditorSelection& m_selection;
	};
}