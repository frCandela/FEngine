#pragma once

#include "core/fanSerializedValues.hpp"
#include "editor/fanEditorGrid.hpp"

namespace fan {
	class MainMenuBar;
	class RenderWindow;
	class SceneWindow;
	class InspectorWindow;
	class PreferencesWindow;
	class ConsoleWindow;
	class EditorWindow;
	class EditorGameWindowCallbacks;
	class EditorSelection;
	class EditorGizmos;
	class EcsWindow;
	class ProfilerWindow;
	class GameWindow;
	class NetworkWindow;
	class SceneInstantiate;
	class Renderer;
	class Gameobject;
	class Scene;
	class EditorCopyPaste;

	//================================================================================================================================
	//================================================================================================================================	
	class Engine {
	public:
		static Signal<Camera*> onSetCamera;

		Engine();
		~Engine();

		void Run();
		void Exit();	

		Renderer &				GetRenderer() const				{ return * m_renderer; }
		MainMenuBar  &			GetMainMenuBar() const			{ return * m_mainMenuBar; }
		RenderWindow &			GetRenderWindow() const			{ return * m_renderWindow; }
		SceneWindow  &			GetSceneWindow() const			{ return * m_sceneWindow; }
		InspectorWindow  &		GetInspectorWindow() const		{ return * m_inspectorWindow; }
		PreferencesWindow  &	GetPreferencesWindow() const	{ return * m_preferencesWindow; }
		ConsoleWindow  &		GetConsoleWindow() const		{ return * m_consoleWindow; }
		Scene &					GetScene() const				{ return * m_currentScene; }
		Scene &					GetServerScene() const			{ return * m_serverScene; }

		EditorGrid GetEditorGrid() const { return m_editorGrid;  }
		void SetEditorGrid( const EditorGrid _editorGrid) { m_editorGrid =_editorGrid; }
		void SetCurrentScene( Scene * _scene );

	private:

		// UI elements
		MainMenuBar *		m_mainMenuBar;

		RenderWindow *		m_renderWindow;
		SceneWindow *		m_sceneWindow;
		InspectorWindow *	m_inspectorWindow;
		PreferencesWindow *	m_preferencesWindow;
		ConsoleWindow *		m_consoleWindow;
		EcsWindow *			m_ecsWindow;
		ProfilerWindow*		m_profilerWindow;
		GameWindow*			m_gameWindow;
		NetworkWindow*		m_networkWindow;
		EditorGrid			m_editorGrid;

		// Main objects
		EditorCopyPaste *				m_copyPaste;
		EditorGameWindowCallbacks	*	m_gameCallbacks;
		EditorSelection *				m_selection;
		EditorGizmos *					m_gizmos;
		Renderer *						m_renderer;
		Scene * 			m_currentScene;
		Scene *				m_clientScene;
		Scene *				m_serverScene;

		bool m_applicationShouldExit;
		bool m_showUI = true;


		void UpdateRenderer();
		void SwitchPlayStop();

		void OnSceneStop( Scene * _scene );
		void OnSceneLoad(Scene * _scene);
		void OnToogleShowUI() { m_showUI = ! m_showUI; }
		void OnToogleView();
		void OnToogleCamera();

		void OnSetCurrentScene( int _scene );

		void DrawEditorGrid() const;
		void DrawWireframe() const;
		void DrawNormals() const;
		void DrawAABB() const;
		void DrawHull() const;
	};
}
