#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
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
		enum Mode{ Client, Server, ClientServer };

		std::string windowName = "FEngine";		// sets the name of the application window
		std::string	loadScene = "";				// loads a scene at startup
		bool		autoPlay = false;			// auto play the scene loaded at startup
		bool		enableLivepp = false;		// enables Live++ hot reload
		bool		mainLoopSleep = false;		// enables sleeping instead of busy waiting in the main loop ( causes frame drops )
		Mode		launchMode = ClientServer;	// launch as server or client
		glm::ivec2  window_position = {-1,-1};	// forces the position of the window
		glm::ivec2  window_size = { -1,-1 };	// forces the size of the window
	};

	//================================================================================================================================
	// base class that contains everything
	// contains a game, a renderer, editions windows/ui and references on EcsWorld
	// One EcsWorld is and instance of the game, there can be multiple EcsWorlds for client and server to run in the same process
	// The m_currentWorld variable is the index of the world that is currently edited
	//================================================================================================================================	
	class Editor
	{
	public:
		Signal <> onLPPSynch;

		Editor( const LaunchSettings _settings, std::vector<EcsWorld*>  _gameWorlds );
		~Editor();

		void Run();
		void Exit();
			   
	private:		
		Renderer* m_renderer;
		Window*   m_window;
		std::vector<EcsWorld*> m_worlds; 
		int m_currentWorld = 0;

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

		EcsWorld& GetCurrentWorld() { return *m_worlds[ m_currentWorld ]; }
		
		
		static void UseEditorCamera( EcsWorld& _world );
		static void UseGameCamera( EcsWorld& _world );

		void OnCurrentGameSwitchPlayStop();
		void OnCurrentGameStart() { GameStart( GetCurrentWorld() ); }
		void OnCurrentGameStop() { GameStop( GetCurrentWorld() ); }
		void OnCurrentGamePause() { GamePause( GetCurrentWorld() ); }
		void OnCurrentGameResume() { GameResume( GetCurrentWorld() ); }
		void OnCurrentGameStep( const float _delta ) { GameStep( GetCurrentWorld(), _delta ); }
		void OnCurrentGameStep();
		void OnCurrentGameToogleCamera();

		static void GameStart( EcsWorld& _world );
		static void GameStop( EcsWorld& _world );
		static void GamePause( EcsWorld& _world );
		static void GameResume( EcsWorld& _world );
		static void GameStep( EcsWorld& _world, const float _delta );		
		static void UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size );

		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { m_showUI = !m_showUI; }
		
		
	};
}
