#pragma once

#include "core/files/fanSerializedValues.h"

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
	class Actor;
	class Camera;
}

namespace fan {

	class Engine {
	public:

		struct EditorGrid {
			bool		isVisible;
			vk::Color	color;
			int			linesCount;
			float		spacing;
		};

		Engine();
		~Engine();

		void Run();
		void Exit();

		inline static Engine & GetEngine() { return * ms_engine; }

		void SetSelectedGameobject( scene::Gameobject * _selectedGameobject) { m_selectedGameobject = _selectedGameobject;	}
		void Deselect() { m_selectedGameobject = nullptr; }

		scene::Gameobject *	const		GetSelectedGameobject() const	{ return m_selectedGameobject;  }
		editor::MainMenuBar  &			GetMainMenuBar() const			{ return * m_mainMenuBar; }
		editor::RenderWindow &			GetRenderWindow() const			{ return * m_renderWindow; }
		editor::SceneWindow  &			GetSceneWindow() const			{ return * m_sceneWindow; }
		editor::InspectorWindow  &		GetInspectorWindow() const		{ return * m_inspectorWindow; }
		editor::PreferencesWindow  &	GetPreferencesWindow() const	{ return * m_preferencesWindow; }
		scene::Scene &					GetScene() const				{ return * m_scene; }
		vk::Renderer &					GetRenderer() const				{ return * m_renderer; }

		fan::SerializedValues & GetEditorValues() { return m_editorValues; }
		EditorGrid GetEditorGrid() const { return m_editorGrid;  }
		void SetEditorGrid( const EditorGrid _editorGrid) { m_editorGrid =_editorGrid; }
		bool DrawMoveGizmo(const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition);

	private:

		// UI elements
		editor::MainMenuBar *		m_mainMenuBar;
		editor::RenderWindow *		m_renderWindow;
		editor::SceneWindow *		m_sceneWindow;
		editor::InspectorWindow *	m_inspectorWindow;
		editor::PreferencesWindow *	m_preferencesWindow;

		EditorGrid m_editorGrid;
		fan::SerializedValues m_editorValues;

		// Main components
		vk::Renderer *		m_renderer;
		scene::Scene *		m_scene;
		scene::Gameobject * m_selectedGameobject;
		scene::Camera *		m_editorCamera;

		struct GizmoCacheData {
			int axisIndex;
			bool pressed = false;
			btVector3 offset;
		};
		std::map< size_t, GizmoCacheData > m_gizmoCacheData;

		static Engine * ms_engine;
		bool m_applicationShouldExit;

		void ManageSelection();
		void OnSceneLoad(scene::Scene * _scene);
		void DrawUI();
		void DrawEditorGrid() const;
		void DrawWireframe() const;
		void DrawAABB() const;

	};
}
