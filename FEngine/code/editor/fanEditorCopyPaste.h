#pragma once

namespace fan
{
	class Engine;

	//================================================================================================================================
	//================================================================================================================================
	class EditorCopyPaste
	{
	public:
		EditorCopyPaste( Engine& _engine );

		// editor callbacks
		void OnCopy();
		void OnPaste();

	private:
		Engine& m_engine;
	};
}