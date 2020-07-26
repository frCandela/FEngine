#pragma once

#include <editor/windows/fanUnitsTestsWindow.hpp>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "render/fanWindow.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"
#include "game/fanLaunchSettings.hpp"

namespace fan
{
	class PreferencesWindow;
	class InspectorWindow;
	class SingletonsWindow;
	class ProfilerWindow;
	class GameViewWindow;
	class ConsoleWindow;
	class NetworkWindow;
	class RenderWindow;
	class EditorWindow;
	class SceneWindow;
	class UnitTestsWindow;
	class EcsWindow;
	class MainMenuBar;
	class EcsWorld;
	class Renderer;
	class Window;
	struct Scene;

	//================================================================================================================================
	// base class that contains everything
	// contains a game, a renderer, editions windows/ui and references on EcsWorld
	// One EcsWorld is and instance of the game, there can be multiple EcsWorlds for client and server to run in the same process
	// The m_currentWorld variable is the index of the world that is currently edited
	//================================================================================================================================	
	class EditorHolder
	{
	public:
		Signal <> onLPPSynch;

		EditorHolder( const LaunchSettings _settings, std::vector<EcsWorld*>  _gameWorlds );
		~EditorHolder();

		void Run();
		void Exit();
		void Step();
			   
	private:		
		Renderer* m_renderer;
		Window   m_window;
		std::vector<EcsWorld*> m_worlds; 
		int m_currentWorld = 0;
		double m_lastRenderTime = 0.;
		const LaunchSettings m_launchSettings;

		// UI elements
        MainMenuBar      *mMainMenuBar;
        PreferencesWindow*mPreferencesWindow;
        SingletonsWindow *mSingletonsWindow;
        InspectorWindow  *mInspectorWindow;
        ProfilerWindow   *mProfilerWindow;
        NetworkWindow    *mNetworkWindow;
        ConsoleWindow    *mConsoleWindow;
        RenderWindow     *mRenderWindow;
        SceneWindow      *mSceneWindow;
        GameViewWindow   *mGameViewWindow;
        EcsWindow        *mEcsWindow;
        UnitTestsWindow  *mUnitTestsWindow;

		bool m_applicationShouldExit;
		bool m_showUI = true;

		EcsWorld& GetCurrentWorld() { return *m_worlds[ m_currentWorld ]; }
		
		static void UseEditorCamera( EcsWorld& _world );
		static void UseGameCamera( EcsWorld& _world );

		void OnCycleCurrentGame();
		void OnCurrentGameSwitchPlayStop();
		void OnCurrentGameStart() { GameStart( GetCurrentWorld() ); }
		void OnCurrentGameStop() { GameStop( GetCurrentWorld() ); }
		void OnCurrentGamePause() { GamePause( GetCurrentWorld() ); }
		void OnCurrentGameResume() { GameResume( GetCurrentWorld() ); }
		void OnCurrentGameStep( const float _delta ) { GameStep( GetCurrentWorld(), _delta ); }
		void OnCurrentGameStep();
		void OnCurrentGameToogleCamera();
		void OnCurrentGameOpen();
		void OnCurrentGameReload();
		void OnCurrentGameSave();
		void OnCurrentGameCopy();
		void OnCurrentGamePaste();
		void OnCurrentGameSelect( const int _index );

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
