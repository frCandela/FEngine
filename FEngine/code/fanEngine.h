#pragma once

namespace vk {
	class Renderer;
}
namespace editor {
	class MainMenuBar;
	class RenderWindow;
	class SceneWindow;
	class InspectorWindow;
}
namespace scene {
	class Scene;
	class Gameobject;
}

namespace fan {
	class Engine {
	public:
		Engine();
		~Engine();

		void Run();
		void Exit();

		static Engine & GetEngine() { return * ms_engine; }

		void SetSelectedGameobject( scene::Gameobject * _selectedGameobject) { m_selectedGameobject = _selectedGameobject;	}
		void Deselect() { m_selectedGameobject = nullptr; }

		scene::Gameobject *	const GetSelectedGameobject() const { return m_selectedGameobject;  }
		editor::MainMenuBar  &	GetMainMenuBar() const			{ return * m_mainMenuBar; }
		editor::RenderWindow &	GetRenderWindow() const			{ return * m_renderWindow; }
		editor::SceneWindow  &	GetSceneWindow() const			{ return * m_sceneWindow; }
		editor::InspectorWindow  &	GetInspector() const		{ return * m_inspectorWindow; }
		scene::Scene &			GetScene() const				{ return * m_scene; }
		vk::Renderer & GetRenderer() const						{ return *m_renderer; }
	private:
		editor::MainMenuBar *		m_mainMenuBar;
		editor::RenderWindow *		m_renderWindow;
		editor::SceneWindow *		m_sceneWindow;
		editor::InspectorWindow *	m_inspectorWindow;

		vk::Renderer * m_renderer;
		scene::Scene * m_scene;
		scene::Gameobject * m_selectedGameobject;

		static Engine * ms_engine;

		bool m_applicationShouldExit;

		void DrawUI();

	};
}
