#pragma once

#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/ecs/fanSignal.hpp"
#include "core/resources/fanResourceManager.hpp"
#include "engine/game/fanLaunchSettings.hpp"
#include "render/fanWindow.hpp"
#include "render/fanRenderer.hpp"
#include "engine/fanFullscreen.hpp"

namespace fan
{
    struct IGame;

    //==================================================================================================================================================================================================
    // contains useful data to make a game player
    //==================================================================================================================================================================================================
    struct PlayerData
    {
        PlayerData( const LaunchSettings& _settings );
        void Destroy();

        Signal<>             mOnLPPSynch;
        const LaunchSettings mLaunchSettings;
        bool                 mApplicationShouldExit;
        Window               mWindow;
        Renderer             mRenderer;
        ResourceManager      mResources;
        FullScreen           mFullScreen;
        FT_Library           mFreetypeLib;

        static void UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size );
        static void MatchFullscreenState( const FullScreen& _fullscreen, Window& _window );
        static void EcsIncludeEngine( EcsWorld& _world );
        static void EcsIncludePhysics( EcsWorld& _world );
        static void EcsIncludeRender3D( EcsWorld& _world );
        static void EcsIncludeRenderUI( EcsWorld& _world );
        static void EcsIncludeNetworkClient( EcsWorld& _world );
        static void EcsIncludeNetworkServer( EcsWorld& _world );

    private:
        static Resource* LoadPrefab( const std::string& _path, ResourceInfo& _info );
        static Resource* LoadFont( const std::string& _path, ResourceInfo& _info );
        static Resource* LoadTexture( const std::string& _path, ResourceInfo& _info );
        static Resource* LoadMesh( const std::string& _path, ResourceInfo& _info );
    };
}
