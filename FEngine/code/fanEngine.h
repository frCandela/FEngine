#pragma once

namespace vk {
	class Renderer;
}
namespace editor {
	class MainMenuBar;
	class RenderWindow;
	class SceneWindow;
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

		scene::Gameobject *		GetSelectedGameobject() const { return m_selectedGameobject;  }
		editor::MainMenuBar  &	GetMainMenuBar() const			{ return * m_mainMenuBar; }
		editor::RenderWindow &	GetRenderWindow() const			{ return * m_renderWindow; }
		editor::SceneWindow  &	GetSceneWindow() const			{ return * m_sceneWindow; }
		scene::Scene &			GetScene() const				{ return * m_scene; }

	private:
		editor::MainMenuBar *	m_mainMenuBar;
		editor::RenderWindow *	m_renderWindow;
		editor::SceneWindow *	m_sceneWindow;

		vk::Renderer * m_renderer;
		scene::Scene * m_scene;
		scene::Gameobject * m_selectedGameobject;

		static Engine * ms_engine;

		bool m_applicationShouldExit;

		void DrawUI();

	};
}
