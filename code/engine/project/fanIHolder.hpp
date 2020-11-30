#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/ecs/fanSignal.hpp"
#include "render/fanWindow.hpp"
#include "render/fanRenderer.hpp"
#include "engine/fanPrefabManager.hpp"
#include "engine/fanFullscreen.hpp"
#include "fanLaunchSettings.hpp"

namespace fan
{
    class IGame;

    //========================================================================================================
    //========================================================================================================
    class IHolder
    {
    public:
        IHolder( const LaunchSettings& _settings );
        virtual ~IHolder() = default;

        void Exit();

        Signal <>            mOnLPPSynch;

    protected:
        const LaunchSettings mLaunchSettings;
        bool                 mApplicationShouldExit;

        Window               mWindow;
        Renderer             mRenderer;
        PrefabManager        mPrefabManager;
        FullScreen           mFullScreen;
        double               mLastRenderTime = 0.;

        void InitWorld( EcsWorld& _world );
        static void UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size );
    };
}
