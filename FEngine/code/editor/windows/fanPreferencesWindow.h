#pragma once

#include "editor/windows/fanEditorWindow.h"
#include "core/input/fanInputManager.h"

namespace fan
{
	class Renderer;

	//================================================================================================================================
	//================================================================================================================================
	class PreferencesWindow : public EditorWindow {
	public:
		PreferencesWindow( Renderer * _renderer );
		~PreferencesWindow();
	protected:
		void OnGui() override;

	private:
		Renderer * m_renderer;

		void DrawJoysticks();
		void DrawShortcuts();
	};
}
