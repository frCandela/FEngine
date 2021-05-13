#pragma once

#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/ecs/fanSignal.hpp"
#include "render/fanWindow.hpp"
#include "render/fanRenderer.hpp"
#include "engine/fanPrefabManager.hpp"
#include "engine/fanFullscreen.hpp"
#include "fanLaunchSettings.hpp"

namespace fan
{
    struct IGame;

    //==================================================================================================================================================================================================
    // contains useful data to make a game player
    //==================================================================================================================================================================================================
    struct PlayerData
    {
        PlayerData( const LaunchSettings& _settings );

        Signal<>             mOnLPPSynch;
        const LaunchSettings mLaunchSettings;
        bool                 mApplicationShouldExit;
        Window               mWindow;
        Renderer             mRenderer;
        PrefabManager        mPrefabManager;
        FullScreen           mFullScreen;

        static void InitWorldResources( EcsWorld& _world, PlayerData& _playerData );
        static void UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size );
        static void MatchFullscreenState( const FullScreen& _fullscreen, Window& _window );
        static void EcsIncludeEngine( EcsWorld& _world );
        static void EcsIncludePhysics( EcsWorld& _world );
        static void EcsIncludeRender3D( EcsWorld& _world );
        static void EcsIncludeRenderUI( EcsWorld& _world );
        static void EcsIncludeNetworkClient( EcsWorld& _world );
        static void EcsIncludeNetworkServer( EcsWorld& _world );
    };
}
