#include <scene/singletons/fanSceneResources.hpp>
#include "fanGameHolder.hpp"

#include "core/fanDebug.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/fanRenderer.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/singletons/fanRenderResources.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	GameHolder::GameHolder( const LaunchSettings _settings, IGame& _game ) :
            mGame( _game )
		, mApplicationShouldExit( false )
		, mLaunchSettings( _settings )
	{
        _game.Init();

		SerializedValues::Get().LoadKeyBindings();
		GameClient::CreateGameAxes();

		// creates window
		glm::ivec2 windowPosition;
		glm::ivec2 windowSize;
		SerializedValues::LoadWindowSizeAndPosition( _settings, windowPosition, windowSize );		
		mWindow.Create( _settings.windowName.c_str(), windowSize, windowPosition );
        Mouse::SetCallbacks( mWindow.mWindow );

		// creates renderer
		mRenderer = new Renderer( mWindow, Renderer::ViewType::Game );
        RenderResources::SetupResources( mRenderer->mMeshManager,
                                         mRenderer->mMesh2DManager,
                                         mRenderer->mTextureManager );

        RenderResources& renderResources = mGame.mWorld.GetSingleton<RenderResources>();
        renderResources.SetPointers(&mRenderer->mMeshManager,
                                    &mRenderer->mMesh2DManager,
                                    &mRenderer->mTextureManager );

        SceneResources::SetupResources( mPrefabManager );
        SceneResources& sceneResources = mGame.mWorld.GetSingleton<SceneResources>();
        sceneResources.SetPointers( &mPrefabManager );

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
	GameHolder::~GameHolder()
	{
		// Serialize editor positions if it was not modified by a launch command
		if( mLaunchSettings.window_size == glm::ivec2( -1, -1 ) )
		{
			const VkExtent2D rendererSize = mWindow.GetExtent();
			const glm::ivec2 windowPosition = mWindow.GetPosition();
            glm::ivec2 size = { rendererSize.width, rendererSize.height };
            SerializedValues::SaveWindowSizeAndPosition( windowPosition, size );
		}

		SerializedValues::Get().SaveValuesToDisk();
		mPrefabManager.Clear();

		delete mRenderer;
		mWindow.Destroy();
	}

	//========================================================================================================
	//========================================================================================================
	void GameHolder::Exit()
	{
        mApplicationShouldExit = true;
	}
	
	//========================================================================================================
	//========================================================================================================
	void GameHolder::Run()
	{
		// initializes timers
		mLastRenderTime = Time::ElapsedSinceStartup();
		Time& time = mGame.mWorld.GetSingleton<Time>();
		time.lastLogicTime = Time::ElapsedSinceStartup();
		
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
		const bool renderIsThisFrame = currentTime > mLastRenderTime + Time::s_renderDelta;
		Time& time = mGame.mWorld.GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      time.lastLogicTime + time.logicDelta;

		// runs logic, renders ui
		while( currentTime > time.lastLogicTime + time.logicDelta )
		{
            mGame.mWorld.GetSingleton<RenderDebug>().Clear();

			// Update input
			ImGui::GetIO().DeltaTime = time.logicDelta;


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
                                                           - ( time.lastLogicTime + time.logicDelta ) );
			if( loopDelayMilliseconds > 30 )
			{
				//Debug::Warning() << "logic is late of " << loopDelayMilliseconds << "ms" << Debug::Endl();
				// if we are really really late, resets the timer
				if( loopDelayMilliseconds > 100 )
				{
					time.lastLogicTime = currentTime - time.logicDelta;
					//Debug::Warning() << "reset logic timer " << Debug::Endl();
				}
			}

			// increase the logic time of a timeScaleDelta with n timeScaleIncrements
			if( std::abs( time.timeScaleDelta ) >= time.timeScaleIncrement )
			{
                const float increment = time.timeScaleDelta > 0.f
                        ? time.timeScaleIncrement
                        : -time.timeScaleIncrement;
                time.lastLogicTime -= increment;
				time.timeScaleDelta -= increment;
			}

			time.lastLogicTime += time.logicDelta;

            mGame.Step( time.logicDelta );

            mGame.mWorld.Run<SMoveFollowTransforms>();

            fanAssert( logicIsThisFrame );

			if( renderIsThisFrame )
			{
				// ImGui render
				ImGui::NewFrame();

                {
                    // in game debug ui here
                    ImGui::Begin( "toto" );
                    ImGuiIO& io = ImGui::GetIO();
                    ImGui::Text( "imMouse %f %f", io.MousePos.x, io.MousePos.y );
                    ImGui::Text( "mouse   %f %f", mouse.mLocalPosition.x, mouse.mLocalPosition.y );
                    ImGui::End();
                }

				ImGui::Render();
			}
            mGame.mWorld.ApplyTransitions();
		}		

		onLPPSynch.Emmit();	

		// Render world		
		if( renderIsThisFrame && logicIsThisFrame )
		{
            mLastRenderTime = currentTime;

			Time::RegisterFrameDrawn();	// used for stats

            UpdateRenderWorld( *mRenderer,
                               mGame.mWorld,
                               { mWindow.GetExtent().width, mWindow.GetExtent().height } );
			mRenderer->DrawFrame();
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

	//========================================================================================================
	// Updates the render world singleton component
	//========================================================================================================
	void GameHolder::UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size )
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		const RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
		renderWorld.targetSize = _size;

		// update render data
		_world.Run<SUpdateRenderWorldModels>();
		_world.Run<SUpdateRenderWorldUI>();
		_world.Run<S_UpdateRenderWorldPointLights>();
		_world.Run<S_UpdateRenderWorldDirectionalLights>();

		// particles mesh
		RenderDataModel particlesDrawData;
		particlesDrawData.mesh = renderWorld.particlesMesh;
		particlesDrawData.modelMatrix = glm::mat4( 1.f );
		particlesDrawData.normalMatrix = glm::mat4( 1.f );
		particlesDrawData.color = glm::vec4( 1.f, 1.f, 1.f, 1.f );
		particlesDrawData.shininess = 1;
		particlesDrawData.textureIndex = 1;
		renderWorld.drawData.push_back( particlesDrawData );

		_renderer.SetDrawData( renderWorld.drawData );
		_renderer.SetUIDrawData( renderWorld.uiDrawData );
		_renderer.SetPointLights( renderWorld.pointLights );
		_renderer.SetDirectionalLights( renderWorld.directionalLights );
        _renderer.SetDebugDrawData( renderDebug.debugLines,
                                    renderDebug.debugLinesNoDepthTest,
                                    renderDebug.debugTriangles );

		// Camera
		Scene& scene = _world.GetSingleton<Scene>();
		EcsEntity cameraID = _world.GetEntity( scene.mainCameraHandle );
		Camera& camera = _world.GetComponent<Camera>( cameraID );
		camera.aspectRatio = _size[0] / _size[1];
		Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
		_renderer.SetMainCamera(
			camera.GetProjection(),
			camera.GetView( cameraTransform ),
			ToGLM( cameraTransform.GetPosition() )
		);
	}
}