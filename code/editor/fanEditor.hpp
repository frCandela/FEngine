#pragma once

#include "core/fanSerializedValues.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	class EditorGameWindowCallbacks;
	class PreferencesWindow;
	class SceneInstantiate;
	class InspectorWindow;
	class EditorCopyPaste;
	class EditorSelection;
	class ProfilerWindow;
	class ConsoleWindow;
	class NetworkWindow;
	class RenderWindow;
	class EditorGizmos;
	class EditorWindow;
	class EcsWorld;
	class SceneWindow;
	class MainMenuBar;
	class EcsWindow;
	class GameWindow;
	class Renderer;
	class Window;
	class Scene;

	//================================================================================================================================
	//================================================================================================================================	
	class Engine
	{
	public:
		Signal <> onLPPSynch;

		Engine();
		~Engine();

		void Run();
		void Exit();

		PreferencesWindow& GetPreferencesWindow() const { return *m_preferencesWindow; }
		InspectorWindow& GetInspectorWindow() const { return *m_inspectorWindow; }
		ConsoleWindow& GetConsoleWindow() const { return *m_consoleWindow; }
		RenderWindow& GetRenderWindow() const { return *m_renderWindow; }
		MainMenuBar& GetMainMenuBar() const { return *m_mainMenuBar; }
		SceneWindow& GetSceneWindow() const { return *m_sceneWindow; }
		
		Scene& GetScene() const { return *m_currentScene; }
		Scene& GetServerScene() const { return *m_serverScene; }
		Renderer& GetRenderer() const { return *m_renderer; }

		void SetCurrentScene( Scene* _scene );

	private:
		EcsWorld m_editorWorld;

		// UI elements
		MainMenuBar* m_mainMenuBar;

		PreferencesWindow* m_preferencesWindow;
		InspectorWindow* m_inspectorWindow;
		ProfilerWindow* m_profilerWindow;
		NetworkWindow* m_networkWindow;
		ConsoleWindow* m_consoleWindow;
		RenderWindow* m_renderWindow;
		SceneWindow* m_sceneWindow;
		GameWindow* m_gameWindow;
		EcsWindow* m_ecsWindow;

		// Main objects
		EditorGameWindowCallbacks* m_gameCallbacks;
		EditorCopyPaste* m_copyPaste;
		EditorSelection* m_selection;
		EditorGizmos* m_gizmos;
		Renderer* m_renderer;
		Window* m_window;
		Scene* m_currentScene;
		Scene* m_clientScene;
		Scene* m_serverScene;

		bool m_applicationShouldExit;
		bool m_showUI = true;

		void UpdateRenderWorld();

		void SwitchPlayStop();

		void OnSceneStop( Scene& _scene );
		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { m_showUI = !m_showUI; }
		void OnToogleView();
		void OnToogleCamera();

		void OnSetCurrentScene( int _scene );

		static void InitializeEcsWorldTypes( EcsWorld& _world );
		static void InitializeEditorEcsWorldTypes( EcsWorld& _world );
	};
}
