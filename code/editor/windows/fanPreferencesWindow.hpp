#pragma once

#include "editor/windows/fanEditorWindow.hpp"
#include "core/input/fanInputManager.hpp"

namespace fan
{
	class Renderer;
    class FullScreen;

	//========================================================================================================
	// shortcut & axis edition
	// imgui colors
	//========================================================================================================
	class PreferencesWindow : public EditorWindow
	{
	public:
		PreferencesWindow( Renderer& _renderer, FullScreen& _fullScreen );
		~PreferencesWindow();

        static void SetDefaultColors();
	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		Renderer&   mRenderer;
        FullScreen& mFullScreen;

		void DrawJoysticks();
		void DrawShortcuts();
        static void LogColorsCppInitCode();
	};
}
