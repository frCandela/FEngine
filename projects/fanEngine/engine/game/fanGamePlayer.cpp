#include "engine/game/fanGamePlayer.hpp"
#include "core/fanDebug.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/game/fanIGame.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    GamePlayer::GamePlayer( LaunchSettings& _settings, IGame& _game ) :
            mData( AdaptSettings( _settings ) ),
            mGame( _game ),
            mLastLogicFrameRendered( 0 )
    {
        EcsWorld& world = mGame.GetWorld();

        PlayerData::EcsIncludeEngine( world );
        PlayerData::EcsIncludePhysics( world );
        PlayerData::EcsIncludeRender3D( world );
        PlayerData::EcsIncludeRenderUI( world );

        world.GetSingleton<Application>().Setup( &mData.mResources );

        Application& app = world.GetSingleton<Application>();
        app.mOnQuit.Connect( &GamePlayer::Exit, this );

        // load scene
        Scene& scene = world.GetSingleton<Scene>();
        scene.New();
        if( !_settings.mLoadScene.empty() )
        {
            scene.LoadFrom( _settings.mLoadScene );
        }

        mGame.Init();
        world.PostInitSingletons();
        mGame.Start();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    LaunchSettings& GamePlayer::AdaptSettings( LaunchSettings& _settings )
    {
        _settings.mIconPath     = RenderGlobal::sGameIcon;
        _settings.mLaunchEditor = false;
        return _settings;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GamePlayer::Run()
    {
        Profiler::Get().Begin();
        while( mData.mApplicationShouldExit == false && mData.mWindow.IsOpen() == true )// main loop
        {
            Step();
        }
        Debug::Log( "Exit application", Debug::Type::Engine );
        mData.Destroy();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GamePlayer::Step()
    {
        EcsWorld& world = mGame.GetWorld();

        const double elapsedTime = Time::ElapsedSinceStartup();
        Time& time = world.GetSingleton<Time>();

        // runs logic, renders ui
        while( elapsedTime > time.mLastLogicTime + time.mLogicDelta.ToDouble() )
        {
            time.mLastLogicTime += time.mLogicDelta.ToDouble();
            time.mFrameIndex++;

            world.GetSingleton<RenderDebug>().Clear();

            mGame.Step( time.mLogicDelta );

            world.Run<SMoveFollowTransforms>();
            world.Run<SUpdateBoundsFromFxSphereColliders>();
            world.Run<SUpdateBoundsFromFxBoxColliders>();
            world.Run<SUpdateBoundsFromModel>();
            world.Run<SUpdateBoundsFromTransform>();

            world.ApplyTransitions();

            world.GetSingleton<Mouse>().ClearSingleFrameEvents();
        }

        mData.mOnLPPSynch.Emmit();

        // Render world
        const double deltaTime = elapsedTime - time.mLastRenderTime;
        if( deltaTime > time.mRenderDelta.ToDouble() )
        {
            ImGui::GetIO().DeltaTime = float( deltaTime );
            time.mLastRenderTime     = elapsedTime;

            // don't draw if logic has not executed/cleared the debug buffers
            if( mLastLogicFrameRendered != time.mFrameIndex )
            {
                mLastLogicFrameRendered = time.mFrameIndex;

                mGame.Render();

                const VkExtent2D extent     = mData.mWindow.GetExtent();
                const glm::vec2  windowSize = glm::vec2( (float)extent.width, (float)extent.height );
                Mouse::NextFrame( mData.mWindow.mWindow, glm::vec2( 0, 0 ), windowSize );
                Input::Get().NewFrame();
                Input::Get().Manager().PullEvents();
                world.GetSingleton<Mouse>().UpdateData( mData.mWindow.mWindow );
            }

            ImGui::NewFrame();
            mGame.OnGui();
            ImGui::Render();

            Time::RegisterFrameDrawn( time, deltaTime );

            PlayerData::UpdateRenderWorld( mData.mRenderer, mGame, { mData.mWindow.GetExtent().width, mData.mWindow.GetExtent().height } );

            mData.mRenderer.DrawFrame();

            PlayerData::MatchCursor( world.GetSingleton<Application>().mCurrentCursor, mData.mWindow );
            PlayerData::MatchFullscreenState( world.GetSingleton<RenderWorld>().mFullscreen, mData.mWindow );

            Profiler::Get().End();
            Profiler::Get().Begin();
        }
    }
}