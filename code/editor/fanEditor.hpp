#pragma once

#include "fanGLM.hpp"
#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"

namespace fan
{
	class EditorGameWindowCallbacks;
	class PreferencesWindow;
	class SceneInstantiate;
	class InspectorWindow;
	class SingletonsWindow;
	class ProfilerWindow;
	class GameViewWindow;
	class ConsoleWindow;
	class NetworkWindow;
	class RenderWindow;
	class EditorWindow;
	class SceneWindow;
	class MainMenuBar;
	class EcsWindow;
	class EcsWorld;
	class Renderer;
	class Window;
	struct Scene;

	//================================================================================================================================
	// the launch parameters of the engine
	//================================================================================================================================
	struct LaunchSettings
	{
		enum Mode{ CLIENT, SERVER};

		std::string	loadScene = "";				// loads a scene at startup
		bool		autoPlay = false;			// auto play the scene loaded at startup
		bool		enableLivepp = false;		// enables Live++ hot reload
		bool		mainLoopSleep = false;		// enables sleeping instead of busy waiting in the main loop ( causes frame drops )
		Mode		launchMode = CLIENT;		// launch as server or client
		glm::ivec2  window_position = {-1,-1};	// forces the position of the window
		glm::ivec2  window_size = { -1,-1 };	// forces the size of the window
	};

	//================================================================================================================================
	// base class that contains everything
	// contains a game, a renderer, editions windows/ui and an editor ecs world (@wip)
	//================================================================================================================================	
	class Editor
	{
	public:
		Signal <> onLPPSynch;

		Editor( const LaunchSettings _settings, EcsWorld& _gameWorld );
		~Editor();

		void Run();
		void Exit();
			   
	private:		
		EcsWorld& m_world;		
		Renderer* m_renderer;
		Window*   m_window;

		const LaunchSettings m_launchSettings;

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
		GameViewWindow* m_gameViewWindow;
		EcsWindow* m_ecsWindow;

		bool m_applicationShouldExit;
		bool m_showUI = true;

		void UpdateRenderWorld();
		void SwitchPlayStop();
		void UseEditorCamera();
		void UseGameCamera();

		void GameStart();
		void GameStop();
		void GamePause();
		void GameResume();
		void GameStep( const float _delta );

		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { m_showUI = !m_showUI; }
		void OnToogleCamera();
		void OnEditorStep();
	};
}
