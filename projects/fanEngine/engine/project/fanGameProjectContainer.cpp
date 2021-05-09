#include "engine/project/fanGameProjectContainer.hpp"
#include "core/fanDebug.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/project/fanIProject.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    GameProjectContainer::GameProjectContainer( LaunchSettings& _settings, IProject& _project ) :
            IProjectContainer( AdaptSettings( _settings ) ),
            mProject( _project ),
            mLastLogicFrameRendered( 0 )
    {
        IProject::EcsIncludeEngine( mProject.mWorld );
        IProject::EcsIncludePhysics( mProject.mWorld );
        IProject::EcsIncludeRender3D( mProject.mWorld );
        IProject::EcsIncludeRenderUI( mProject.mWorld );

        SerializedValues::Get().LoadKeyBindings();

        InitWorld( mProject.mWorld );

        Application& app = mProject.mWorld.GetSingleton<Application>();
        app.mOnQuit.Connect( &IProjectContainer::Exit, (IProjectContainer*)this );

        // load scene
        Scene& scene = mProject.mWorld.GetSingleton<Scene>();
        scene.New();
        if( !_settings.mLoadScene.empty() )
        {
            scene.LoadFrom( _settings.mLoadScene );
        }

        mProject.Init();
        mProject.mWorld.PostInitSingletons();
        mProject.Start();
    }

    //========================================================================================================
    //========================================================================================================
    LaunchSettings& GameProjectContainer::AdaptSettings( LaunchSettings& _settings )
    {
        if( !_settings.mForceWindowDimensions )
        {
            SerializedValues::LoadWindowPosition( _settings.mWindow_position );
            SerializedValues::LoadWindowSize( _settings.mWindow_size );
        }
        _settings.mIconPath     = RenderGlobal::sGameIcon;
        _settings.mLaunchEditor = false;
        return _settings;
    }

    //========================================================================================================
    //========================================================================================================
    void GameProjectContainer::Run()
    {
        Profiler::Get().Begin();
        while( mApplicationShouldExit == false && mWindow.IsOpen() == true )// main loop
        {
            Step();
        }
        Debug::Log( "Exit application" );
    }

    //========================================================================================================
    //========================================================================================================
    void GameProjectContainer::Step()
    {
        const double elapsedTime = Time::ElapsedSinceStartup();
        Time& time = mProject.mWorld.GetSingleton<Time>();

        // runs logic, renders ui
        while( elapsedTime > time.mLastLogicTime + time.mLogicDelta.ToDouble() )
        {
            time.mLastLogicTime += time.mLogicDelta.ToDouble();
            time.mFrameIndex++;

            mProject.mWorld.GetSingleton<RenderDebug>().Clear();

            mProject.Step( time.mLogicDelta );

            mProject.mWorld.Run<SMoveFollowTransforms>();
            mProject.mWorld.Run<SUpdateBoundsFromFxSphereColliders>();
            mProject.mWorld.Run<SUpdateBoundsFromFxBoxColliders>();
            mProject.mWorld.Run<SUpdateBoundsFromModel>();
            mProject.mWorld.Run<SUpdateBoundsFromTransform>();

            mProject.mWorld.ApplyTransitions();
        }

        mOnLPPSynch.Emmit();

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

                mProject.Render();

                const VkExtent2D extent     = mWindow.GetExtent();
                const glm::vec2  windowSize = glm::vec2( (float)extent.width, (float)extent.height );
                Mouse::NextFrame( mWindow.mWindow, glm::vec2( 0, 0 ), windowSize );
                Input::Get().NewFrame();
                Input::Get().Manager().PullEvents();
                mProject.mWorld.GetSingleton<Mouse>().UpdateData( mWindow.mWindow );
            }

            ImGui::NewFrame();
            mProject.OnGui();
            ImGui::Render();

            Time::RegisterFrameDrawn( time, deltaTime );

            UpdateRenderWorld( mRenderer, mProject, { mWindow.GetExtent().width, mWindow.GetExtent().height } );

            mRenderer.DrawFrame();

            Profiler::Get().End();
            Profiler::Get().Begin();
        }
    }
}