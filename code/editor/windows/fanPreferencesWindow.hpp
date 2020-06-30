#pragma once

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
		PreferencesWindow( Renderer& _renderer );
		~PreferencesWindow();
	
	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		Renderer& m_renderer;

		void DrawJoysticks();
		void DrawShortcuts();
	};
}
