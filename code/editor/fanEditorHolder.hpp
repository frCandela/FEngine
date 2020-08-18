#pragma once

#include <editor/windows/fanUnitsTestsWindow.hpp>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "render/fanWindow.hpp"
#include "scene/fanPrefabManager.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"
#include "game/fanLaunchSettings.hpp"

namespace fan
{
	class GameViewWindow;
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
		Signal <> mOnLPPSynch;

		EditorHolder( const LaunchSettings _settings, std::vector<GameBase*>  _games );
		~EditorHolder();

		void Run();
		void Exit();
		void Step();
			   
	private:		
		Renderer*               mRenderer;
		Window                  mWindow;
		PrefabManager           mPrefabManager;
		std::vector<GameBase*>  mGames;
		int                     mCurrentGame    = 0;
		double                  mLastRenderTime = 0.;
		const LaunchSettings    mLaunchSettings;
        bool                    mApplicationShouldExit;
        bool                    mShowUi = true;

        MainMenuBar      *mMainMenuBar;
        GameViewWindow   *mGameViewWindow;

		GameBase& GetCurrentGame() { return *mGames[ mCurrentGame ]; }
		
		static void UseEditorCamera( EcsWorld& _world );
		static void UseGameCamera( EcsWorld& _world );

		void OnCycleCurrentGame();
		void OnCurrentGameSwitchPlayStop();
		void OnCurrentGameStart() { GameStart( GetCurrentGame() ); }
		void OnCurrentGameStop() { GameStop( GetCurrentGame() ); }
		void OnCurrentGamePause() { GamePause( GetCurrentGame() ); }
		void OnCurrentGameResume() { GameResume( GetCurrentGame() ); }
		void OnCurrentGameStep( const float _delta ) { GameStep( GetCurrentGame(), _delta ); }
		void OnCurrentGameStep();
		void OnCurrentGameToogleCamera();
		void OnCurrentGameOpen();
		void OnCurrentGameReload();
		void OnCurrentGameSave();
		void OnCurrentGameCopy();
		void OnCurrentGamePaste();
		void OnCurrentGameSelect( const int _index );

		static void GameStart(  GameBase& _game );
		static void GameStop(   GameBase& _game );
		static void GamePause(  GameBase& _game );
		static void GameResume( GameBase& _game );
		static void GameStep(   GameBase& _game, const float _delta );
		static void UpdateRenderWorld( Renderer& _renderer, GameBase& _game, const glm::vec2 _size );

		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { mShowUi = !mShowUi; }
	};
}
