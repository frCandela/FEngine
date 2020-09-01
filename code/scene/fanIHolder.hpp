#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "ecs/fanSignal.hpp"
#include "render/fanWindow.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanFullscreen.hpp"
#include "game/fanLaunchSettings.hpp"

namespace fan
{
    class Renderer;
    class IGame;

    //========================================================================================================
    //========================================================================================================
    class IHolder
    {
    public:
        IHolder( const LaunchSettings& _settings ) :
                mLaunchSettings( _settings ), mApplicationShouldExit( false ) {}
        virtual ~IHolder();

        void Exit();

        Signal <>            mOnLPPSynch;

    protected:
        const LaunchSettings mLaunchSettings;
        bool                 mApplicationShouldExit;

        Renderer*            mRenderer;
        Window               mWindow;
        PrefabManager        mPrefabManager;
        FullScreen           mFullScreen;
        double               mLastRenderTime = 0.;

        static void UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size );
    };
}
