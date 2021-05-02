#include "engine/project/fanGameProjectContainer.hpp"
#include "core/fanDebug.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
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
            mProject( _project )
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
        mProject.Start();
	}

    //========================================================================================================
    //========================================================================================================
    LaunchSettings& GameProjectContainer::AdaptSettings( LaunchSettings& _settings )
    {
        if( ! _settings.mForceWindowDimensions )
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
		// initializes timers
		mLastRenderTime = Time::ElapsedSinceStartup();
		Time& time = mProject.mWorld.GetSingleton<Time>();
		time.mLastLogicTime = Time::ElapsedSinceStartup();
		
		Profiler::Get().Begin();

		// main loop
		while( mApplicationShouldExit == false && mWindow.IsOpen() == true )
		{
			Step();
		}

		Debug::Log( "Exit application" );
	}
	
	//========================================================================================================
	//========================================================================================================
	void GameProjectContainer::Step()
	{
		const double currentTime = Time::ElapsedSinceStartup();
		const bool renderIsThisFrame = currentTime > mLastRenderTime + Time::sRenderDelta;
		Time& time = mProject.mWorld.GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      time.mLastLogicTime + time.mLogicDelta;

		// runs logic, renders ui
		while( currentTime > time.mLastLogicTime + time.mLogicDelta )
		{
            mProject.mWorld.GetSingleton<RenderDebug>().Clear();

			// Update input
			ImGui::GetIO().DeltaTime = time.mLogicDelta;

			const VkExtent2D extent = mWindow.GetExtent();
            const glm::vec2 windowSize = glm::vec2( (float)extent.width, (float)extent.height );

            Mouse::NextFrame( mWindow.mWindow, glm::vec2( 0, 0) , windowSize ); /*todo true window hovered*/
            Input::Get().NewFrame();
            Input::Get().Manager().PullEvents();
            Mouse& mouse = mProject.mWorld.GetSingleton<Mouse>();
            mouse.UpdateData( mWindow.mWindow );

			// checking the loop timing is not late
            const double loopDelayMilliseconds = 1000. * ( currentTime
                                                           - ( time.mLastLogicTime + time.mLogicDelta ) );
			if( loopDelayMilliseconds > 30 )
			{
				//Debug::Warning() << "logic is late of " << loopDelayMilliseconds << "ms" << Debug::Endl();
				// if we are really really late, resets the timer
				if( loopDelayMilliseconds > 100 )
				{
					time.mLastLogicTime = currentTime - time.mLogicDelta;
					//Debug::Warning() << "reset logic timer " << Debug::Endl();
				}
			}

			// increase the logic time of a timeScaleDelta with n timeScaleIncrements
			if( std::abs( time.mTimeScaleDelta ) >= time.mTimeScaleIncrement )
			{
                const float increment = time.mTimeScaleDelta > 0.f
                        ? time.mTimeScaleIncrement
                        : -time.mTimeScaleIncrement;
                time.mLastLogicTime -= increment;
				time.mTimeScaleDelta -= increment;
			}

			time.mLastLogicTime += time.mLogicDelta;

            mProject.Step( time.mLogicDelta );

            mProject.mWorld.Run<SMoveFollowTransforms>();

            fanAssert( logicIsThisFrame );

			if( renderIsThisFrame )
			{
				// ImGui render
				ImGui::NewFrame();
				mProject.OnGui();

                ImGui::Begin( "test" );
                mFullScreen.OnGui( mWindow );

                ImGui::Text("%f %f", mouse.mLocalPosition.x, mouse.mLocalPosition.y );

                ImGui::End();

				ImGui::Render();
			}
            mProject.mWorld.ApplyTransitions();
		}

        mOnLPPSynch.Emmit();

		// Render world		
		if( renderIsThisFrame && logicIsThisFrame )
		{
            mLastRenderTime = currentTime;

			Time::RegisterFrameDrawn();	// used for stats

            UpdateRenderWorld( mRenderer,
                               mProject,
                               { mWindow.GetExtent().width, mWindow.GetExtent().height } );
			mRenderer.DrawFrame();
			Profiler::Get().End();
			Profiler::Get().Begin();
		}

		// sleep for the rest of the frame
		if( mLaunchSettings.mMainLoopSleep )
		{
			// @todo repair this to work with multiple worlds running 
// 				const double minSleepTime = 1;
// 				const double endFrameTime = Time::ElapsedSinceStartup();
// 				const double timeBeforeNextLogic = lastLogicTime + time.logicDelta - endFrameTime;
// 				const double timeBeforeNextRender = lastRenderTime + Time::s_renderDelta - endFrameTime;
// 				const double sleepTimeMiliseconds = 1000. * std::min( timeBeforeNextLogic, timeBeforeNextRender );
// 				if( sleepTimeMiliseconds > minSleepTime )
// 				{
// 					std::this_thread::sleep_for( std::chrono::milliseconds( int( sleepTimeMiliseconds / 2 ) ) );
// 				}
		}
	}
}