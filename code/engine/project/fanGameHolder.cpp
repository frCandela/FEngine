#include "engine/project/fanGameHolder.hpp"
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
#include "engine/project/fanIGame.hpp"

namespace fan
{
	//========================================================================================================
    //========================================================================================================
    GameHolder::GameHolder( LaunchSettings& _settings, IGame& _game ) :
            IHolder( AdaptSettings( _settings ) ),
            mGame( _game )
    {
        _game.Init();

		SerializedValues::Get().LoadKeyBindings();

		InitWorld( _game.mWorld );

        Application& app = _game.mWorld.GetSingleton<Application>();
        app.mOnQuit.Connect( &IHolder::Exit, (IHolder*)this );

		// load scene
		Scene& scene = mGame.mWorld.GetSingleton<Scene>();
		scene.New();
		if( !_settings.loadScene.empty() )
		{
			scene.LoadFrom( _settings.loadScene );
            mGame.Start();
        }
	}

    //========================================================================================================
    //========================================================================================================
    LaunchSettings& GameHolder::AdaptSettings( LaunchSettings& _settings )
    {
        if( ! _settings.mForceWindowDimensions )
        {
            SerializedValues::LoadWindowPosition( _settings.window_position );
            SerializedValues::LoadWindowSize( _settings.window_size );
        }
        _settings.mIconPath = RenderGlobal::sGameIcon;
        return _settings;
    }

	//========================================================================================================
	//========================================================================================================
	void GameHolder::Run()
	{
		// initializes timers
		mLastRenderTime = Time::ElapsedSinceStartup();
		Time& time = mGame.mWorld.GetSingleton<Time>();
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
	void GameHolder::Step()
	{
		const double currentTime = Time::ElapsedSinceStartup();
		const bool renderIsThisFrame = currentTime > mLastRenderTime + Time::sRenderDelta;
		Time& time = mGame.mWorld.GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      time.mLastLogicTime + time.mLogicDelta;

		// runs logic, renders ui
		while( currentTime > time.mLastLogicTime + time.mLogicDelta )
		{
            mGame.mWorld.GetSingleton<RenderDebug>().Clear();

			// Update input
			ImGui::GetIO().DeltaTime = time.mLogicDelta;


			const glm::ivec2 iPos = mWindow.GetPosition();
            const glm::vec2 windowPosition = glm::vec2( (float)iPos.x, (float)iPos.y );
			const VkExtent2D extent = mWindow.GetExtent();
            const glm::vec2 windowSize = glm::vec2( (float)extent.width, (float)extent.height );

            Mouse::NextFrame( mWindow.mWindow, glm::vec2( 0, 0) , windowSize ); /*todo true window hovered*/
            Input::Get().NewFrame();
            Input::Get().Manager().PullEvents();
            Mouse& mouse = mGame.mWorld.GetSingleton<Mouse>();
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

            mGame.Step( time.mLogicDelta );

            mGame.mWorld.Run<SMoveFollowTransforms>();

            fanAssert( logicIsThisFrame );

			if( renderIsThisFrame )
			{
				// ImGui render
				ImGui::NewFrame();

                ImGui::Begin( "test" );
                mFullScreen.OnGui( mWindow );

                ImGui::Text("%f %f", mouse.mLocalPosition.x, mouse.mLocalPosition.y );

                ImGui::End();

				ImGui::Render();
			}
            mGame.mWorld.ApplyTransitions();
		}

        mOnLPPSynch.Emmit();

		// Render world		
		if( renderIsThisFrame && logicIsThisFrame )
		{
            mLastRenderTime = currentTime;

			Time::RegisterFrameDrawn();	// used for stats

            UpdateRenderWorld( mRenderer,
                               mGame,
                               { mWindow.GetExtent().width, mWindow.GetExtent().height } );
			mRenderer.DrawFrame();
			Profiler::Get().End();
			Profiler::Get().Begin();
		}

		// sleep for the rest of the frame
		if( mLaunchSettings.mainLoopSleep )
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