#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class EditorSelection;

	//================================================================================================================================
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