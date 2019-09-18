#pragma once

#include "core/files/fanSerializedValues.h"

namespace fan {
	 class Renderer;

	namespace editor {
		class MainMenuBar;
		class RenderWindow;
		class SceneWindow;
		class InspectorWindow;
		class PreferencesWindow;
		class ConsoleWindow;
	}
	namespace scene {
		class Scene;
		class Entity;
		class Actor;
		class Camera;
	}

	//================================================================================================================================
	//================================================================================================================================	
	class Engine {
	public:

		struct EditorGrid {
			bool		isVisible;
			Color	color;
			int			linesCount;
			float		spacing;
		};

		Engine();
		~Engine();

		void Run();
		void Exit();

		inline static Engine & GetEngine() { return * ms_engine; }

		void SetSelectedEntity( scene::Entity * _selectedentity) { m_selectedentity = _selectedentity;	}
		void Deselect() { m_selectedentity = nullptr; }

		scene::Entity *	const			GetSelectedentity() const		{ return m_selectedentity;  }
		scene::Camera *					GetEditorCamera() const			{ return m_editorCamera; }
		editor::MainMenuBar  &			GetMainMenuBar() const			{ return * m_mainMenuBar; }
		editor::RenderWindow &			GetRenderWindow() const			{ return * m_renderWindow; }
		editor::SceneWindow  &			GetSceneWindow() const			{ return * m_sceneWindow; }
		editor::InspectorWindow  &		GetInspectorWindow() const		{ return * m_inspectorWindow; }
		editor::PreferencesWindow  &	GetPreferencesWindow() const	{ return * m_preferencesWindow; }
		editor::ConsoleWindow  &		GetConsoleWindow() const		{ return * m_consoleWindow; }
		scene::Scene &					GetScene() const				{ return * m_scene; }

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
		editor::ConsoleWindow *		m_consoleWindow;

		EditorGrid m_editorGrid;
		fan::SerializedValues m_editorValues;

		// Main components
		scene::Scene *	m_scene;
		scene::Entity * m_selectedentity;
		scene::Camera *	m_editorCamera;

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
		void DrawNormals() const;
		void DrawAABB() const;
		void DrawHull() const;

	};
}
