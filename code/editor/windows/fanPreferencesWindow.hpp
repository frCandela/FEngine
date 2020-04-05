#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "core/input/fanInputManager.hpp"

namespace fan
{
	class Renderer;

	//================================================================================================================================
	// shortcut & axis edition
	// imgui colors
	//================================================================================================================================
	class PreferencesWindow : public EditorWindow
	{
	public:
		PreferencesWindow();
		~PreferencesWindow();

		void SetRenderer( Renderer* _renderer ) { m_renderer = _renderer; }
	protected:
		void OnGui() override;

	private:
		Renderer* m_renderer;

		void DrawJoysticks();
		void DrawShortcuts();
	};
}
