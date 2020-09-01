#pragma once

#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/fanIHolder.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"

namespace fan
{
	class GameViewWindow;
	class MainMenuBar;
	class Window;
	struct Scene;

	//========================================================================================================
	// base class that contains everything
	// contains a game, a renderer, editions windows/ui and references on EcsWorld
	// here can be multiple IGame for client and server to run in the same process
	//========================================================================================================
	class EditorHolder : public IHolder
	{
	public:
		EditorHolder( LaunchSettings& _settings, const std::vector<IGame*>& _games );
		~EditorHolder() override;

		void Run();
		void Step();
	private:
		std::vector<IGame*>  mGames;
		int                  mCurrentGame    = 0;
        bool                 mShowUi = true;
        MainMenuBar *        mMainMenuBar;
        GameViewWindow *     mGameViewWindow;

		IGame& GetCurrentGame() { return *mGames[ mCurrentGame ]; }
        LaunchSettings& AdaptSettings( LaunchSettings& _settings );
		static void UseEditorCamera( EcsWorld& _world );

		void OnCycleCurrentGame();
		void OnCurrentGameSwitchPlayStop();
		void OnCurrentGameStart();
		void OnCurrentGameStop() { GameStop( GetCurrentGame() ); }
		void OnCurrentGamePause() { GamePause( GetCurrentGame() ); }
		void OnCurrentGameResume() { GameResume( GetCurrentGame() ); }
		void OnCurrentGameStep();
		void OnCurrentGameToogleCamera();
		void OnCurrentGameOpen();
		void OnCurrentGameReload();
		void OnCurrentGameSave();
		void OnCurrentGameCopy();
		void OnCurrentGamePaste();
        void OnCurrentGameDeleteSelection();
        void OnCurrentGameToogleTransformLock();
		void OnCurrentGameSelect( const int _index );

		static void GameStart( IGame& _game );
		static void GameStop( IGame& _game );
		static void GamePause( IGame& _game );
		static void GameResume( IGame& _game );

		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { mShowUi = !mShowUi; }
	};
}
