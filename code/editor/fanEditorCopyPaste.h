#pragma once

namespace fan
{
	class EditorSelection;

	//================================================================================================================================
	//================================================================================================================================
	class EditorCopyPaste
	{
	public:
		EditorCopyPaste( EditorSelection& _engine );

		// editor callbacks
		void OnCopy();
		void OnPaste();

	private:
		EditorSelection& m_selection;
	};
}