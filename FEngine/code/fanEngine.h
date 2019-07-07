#pragma once

namespace vk {
	class Renderer;
}
namespace editor {
	class MainMenuBar;
	class RenderWindow;
	class SceneWindow;
	class InspectorWindow;
	class PreferencesWindow;
}
namespace scene {
	class Scene;
	class Gameobject;
}

namespace fan {
	class Engine {
	public:

		struct EditorGrid {
			glm::vec4	color;
			int			linesCount;
			float		spacing;
		};

		Engine();
		~Engine();

		void Run();
		void Exit();

		static Engine & GetEngine() { return * ms_engine; }

		void SetSelectedGameobject( scene::Gameobject * _selectedGameobject) { m_selectedGameobject = _selectedGameobject;	}
		void Deselect() { m_selectedGameobject = nullptr; }

		scene::Gameobject *	const GetSelectedGameobject() const		{ return m_selectedGameobject;  }
		editor::MainMenuBar  &	GetMainMenuBar() const				{ return * m_mainMenuBar; }
		editor::RenderWindow &	GetRenderWindow() const				{ return * m_renderWindow; }
		editor::SceneWindow  &	GetSceneWindow() const				{ return * m_sceneWindow; }
		editor::InspectorWindow  &	GetInspectorWindow() const		{ return * m_inspectorWindow; }
		editor::PreferencesWindow  & GetPreferencesWindow() const	{ return * m_preferencesWindow; }
		scene::Scene &			GetScene() const					{ return * m_scene; }
		vk::Renderer & GetRenderer() const							{ return * m_renderer; }


		EditorGrid GetEditorGrid() const { return m_editorGrid;  }
		void SetEditorGrid( const EditorGrid _editorGrid) { m_editorGrid =_editorGrid; }

	private:
		editor::MainMenuBar *		m_mainMenuBar;
		editor::RenderWindow *		m_renderWindow;
		editor::SceneWindow *		m_sceneWindow;
		editor::InspectorWindow *	m_inspectorWindow;
		editor::PreferencesWindow *	m_preferencesWindow;

		vk::Renderer * m_renderer;
		scene::Scene * m_scene;
		scene::Gameobject * m_selectedGameobject;

		static Engine * ms_engine;

		bool m_applicationShouldExit;

		// Scene grid
		EditorGrid m_editorGrid;

		void DrawUI();
		void DrawEditorGrid();
	};
}
