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
    class IProject;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class IProjectContainer
    {
    public:
        IProjectContainer( const LaunchSettings& _settings );
        virtual ~IProjectContainer() = default;

        virtual void Run() = 0;
        void Exit();

        Signal<> mOnLPPSynch;

    protected:
        const LaunchSettings mLaunchSettings;
        bool                 mApplicationShouldExit;

        Window        mWindow;
        Renderer      mRenderer;
        PrefabManager mPrefabManager;
        FullScreen    mFullScreen;

        void InitWorld( EcsWorld& _world );
        static void UpdateRenderWorld( Renderer& _renderer, IProject& _project, const glm::vec2 _size );
    };
}
