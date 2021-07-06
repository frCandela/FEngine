#pragma once

#include "network/fanNetConfig.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "engine/game/fanPlayerData.hpp"
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanFilesWatcher.hpp"

namespace fan
{
    class Window;
    struct Scene;

    //==================================================================================================================================================================================================
    // Hold one or more games to play in an editor
    // Contains lots of tools and windows for editing the games
    //==================================================================================================================================================================================================
    struct EditorPlayer
    {
        std::vector<IGame*> mGames;
        int                 mCurrentGame           = 0;
        bool                mShowUi                = true;
        FrameIndex          mLastLogicFrameRendered;
        bool                mStopPlayingEndOfFrame = false;
        EditorSettingsData  mEditorSettings;
        PlayerData          mData;
        FilesWatcher        mFilesWatcher;

        IGame& GetCurrentGame() { return *mGames[mCurrentGame]; }
        LaunchSettings& AdaptSettings( LaunchSettings& _settings );
        void HotReloadFiles();

        EditorPlayer( LaunchSettings& _settings, const std::vector<IGame*>& _games );
        void Run();
        void Step();
        void Exit() { mData.mApplicationShouldExit = true; };

        void OnCycleCurrentGame();
        void OnSwitchPlayStop();
        void OnStart();
        void OnStop() { mStopPlayingEndOfFrame = true; }
        void OnPause() { Pause( GetCurrentGame() ); }
        void OnResume() { Resume( GetCurrentGame() ); }
        void OnStep();
        void OnToogleCamera();
        void OnOpen();
        void OnReload();
        void OnSave();
        void OnCopy();
        void OnPaste();
        void OnDeleteSelection();
        void OnToogleTransformLock();
        void OnSelect( const int _index );
        void OnSceneLoad( Scene& _scene );
        void OnToggleShowUI() { mShowUi = !mShowUi; }

        static void Start( IGame& _game );
        static void Stop( IGame& _game );
        static void Pause( IGame& _game );
        static void Resume( IGame& _game );
        static void UseEditorCamera( EcsWorld& _world );
        static void DrawEditorUI( EcsWorld& _world );
        static void EcsIncludeEditor( EcsWorld& _world );
    };
}
