#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Renderer;
	//================================================================================================================================
	//================================================================================================================================
	class PreferencesWindow : public EditorWindow {
	public:
		PreferencesWindow( Renderer * _renderer );

	protected:
		void OnGui() override;

	private:
		Renderer * m_renderer;
	};
}
