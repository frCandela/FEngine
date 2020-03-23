#pragma once

#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/fanGame.hpp"

namespace fan
{
	class EditorGameWindowCallbacks;
	class PreferencesWindow;
	class SceneInstantiate;
	class InspectorWindow;
	class EditorCopyPaste;
	class SingletonsWindow;
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
			   
	private:
		EcsWorld m_editorWorld;
		Game*	 m_game;

		// UI elements
		MainMenuBar* m_mainMenuBar;

		PreferencesWindow* m_preferencesWindow;
		SingletonsWindow* m_singletonsWindow;
		InspectorWindow* m_inspectorWindow;
		ProfilerWindow* m_profilerWindow;
		NetworkWindow* m_networkWindow;
		ConsoleWindow* m_consoleWindow;
		RenderWindow* m_renderWindow;
		SceneWindow* m_sceneWindow;
		GameWindow* m_gameWindow;
		EcsWindow* m_ecsWindow;

		// Main objects
		EditorCopyPaste* m_copyPaste;
		EditorSelection* m_selection;
		EditorGizmos* m_gizmos;
		Renderer* m_renderer;
		Window* m_window;

		bool m_applicationShouldExit;
		bool m_showUI = true;

		void UpdateRenderWorld();
		void SwitchPlayStop();
		void UseEditorCamera();
		void UseGameCamera();

		void OnGamePlay();
		void OnGamePause();
		void OnGameResume();
		void OnGameStop();
		void OnGameStep();
		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { m_showUI = !m_showUI; }
		void OnToogleCamera();

		static void InitializeEditorEcsWorldTypes( EcsWorld& _world );
		static void InitializeGameEcsWorldTypes( EcsWorld& _world );
	};
}
