#include "fanGameHolder.hpp"

#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/input/fanMouse.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/util/fanWindow.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanRendererDebug.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/fanPrefab.hpp"
#include "game/singletons/fanGameCamera.hpp"
#include "game/singletons/fanGame.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameHolder::GameHolder( const LaunchSettings _settings, EcsWorld& _world ) :
		m_world( _world )
		, m_applicationShouldExit( false )
		, m_launchSettings( _settings )
	{
		SerializedValues::Get().LoadKeyBindings();
		GameClient::CreateGameAxes();

		// creates window
		glm::ivec2 windowPosition;
		glm::ivec2 windowSize;
		SerializedValues::LoadWindowSizeAndPosition( _settings, windowPosition, windowSize );		
		m_window = new Window( _settings.windowName.c_str(), windowSize, windowPosition );

		// creates renderer
		m_renderer = new Renderer( *m_window );

		Prefab::s_resourceManager.Init();

		// load scene
		Scene& scene = m_world.GetSingleton<Scene>();
		scene.New();
		if( !_settings.loadScene.empty() )
		{
			scene.LoadFrom( _settings.loadScene );

			// auto play the scene
			GameStart( m_world );			
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	GameHolder::~GameHolder()
	{
		// Serialize editor positions if it was not modified by a launch command
		if( m_launchSettings.window_size == glm::ivec2( -1, -1 ) )
		{
			const VkExtent2D rendererSize = m_window->GetExtent();
			const glm::ivec2 windowPosition = m_window->GetPosition();
			SerializedValues::SaveWindowSizeAndPosition( windowPosition, { rendererSize.width, rendererSize.height } );
		}

		SerializedValues::Get().SaveValuesToDisk();

		Prefab::s_resourceManager.Clear();
		delete m_renderer;
		delete m_window;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameHolder::Exit()
	{
		m_applicationShouldExit = true;
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void GameHolder::Run()
	{
		// initializes timers
		m_lastRenderTime = Time::ElapsedSinceStartup();
		Time& time = m_world.GetSingleton<Time>();
		time.lastLogicTime = Time::ElapsedSinceStartup();
		
		Profiler::Get().Begin();

		// main loop
		while( m_applicationShouldExit == false && m_window->IsOpen() == true )
		{
			Step();
		}

		// Exit sequence
		Debug::Log( "Exit application" );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void GameHolder::Step()
	{
		const double currentTime = Time::ElapsedSinceStartup();
		const bool renderIsThisFrame = currentTime > m_lastRenderTime + Time::s_renderDelta;
		const Time& currentWorldTime = m_world.GetSingleton<Time>();
		const bool logicIsThisFrame = currentTime > currentWorldTime.lastLogicTime + currentWorldTime.logicDelta;

		// Update all worlds

		// runs logic, renders ui
		Time& time = m_world.GetSingleton<Time>();
		while( currentTime > time.lastLogicTime + time.logicDelta )
		{
			m_world.GetSingleton<RenderDebug>().Clear();

			// Update input
			ImGui::GetIO().DeltaTime = time.logicDelta;
			Input::Get().NewFrame();

			const glm::ivec2 position = m_window->GetPosition();
			const VkExtent2D extent = m_window->GetExtent();


			Mouse::Get().Update( btVector2( (float)position.x, (float)position.y ), btVector2( (float)extent.width, (float)extent.height ), true );
			Input::Get().Manager().PullEvents();			

			// checking the loop timing is not late
			const double loopDelayMilliseconds = 1000. * ( currentTime - ( time.lastLogicTime + time.logicDelta ) );
			if( loopDelayMilliseconds > 30 )
			{
				Debug::Warning() << "logic is late of " << loopDelayMilliseconds << "ms" << Debug::Endl();
				// if we are really really late, resets the timer
				if( loopDelayMilliseconds > 100 )
				{
					time.lastLogicTime = currentTime - time.logicDelta;
					Debug::Warning() << "reset logic timer " << Debug::Endl();
				}
			}

			// increase the logic time of a timeScaleDelta with n timeScaleIncrements
			if( std::abs( time.timeScaleDelta ) >= time.timeScaleIncrement )
			{
				const float increment = time.timeScaleDelta > 0.f ? time.timeScaleIncrement : -time.timeScaleIncrement;
				time.lastLogicTime -= increment;
				time.timeScaleDelta -= increment;
			}

			time.lastLogicTime += time.logicDelta;

			GameStep( m_world, time.logicDelta );

			m_world.Run<S_MoveFollowTransforms>();
			m_world.Run<S_MoveFollowTransformsUI>();			

			assert( logicIsThisFrame );

			if( renderIsThisFrame )
			{
				// ImGui render
				ImGui::NewFrame();
				// in game debug ui here
				ImGui::Begin( "toto" );
				ImGui::Text( "hello!" );
				ImGui::End();
				ImGui::Render();
			}			
			m_world.ApplyTransitions();
		}		

		onLPPSynch.Emmit();	

		// Render world		
		if( renderIsThisFrame && logicIsThisFrame )
		{
			m_lastRenderTime = currentTime;

			Time::RegisterFrameDrawn();	// used for stats
			
			UpdateRenderWorld( *m_renderer, m_world, { m_window->GetExtent().width, m_window->GetExtent().height } );
			const RenderDebug& renderDebug = m_world.GetSingleton<RenderDebug>();

			m_renderer->GetRendererDebug().UpdateDebugBuffer( renderDebug.debugLines, renderDebug.debugLinesNoDepthTest, renderDebug.debugTriangles );
			m_renderer->DrawFrame();
			Profiler::Get().End();
			Profiler::Get().Begin();
		}

		// sleep for the rest of the frame
		if( m_launchSettings.mainLoopSleep )
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

	//================================================================================================================================
	//================================================================================================================================
	void GameHolder::GameStart( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		assert( game.state == Game::STOPPED );
		// saves the scene before playing
		Scene& scene = _world.GetSingleton<Scene>();
		assert( !scene.path.empty() );
		scene.Save();
		Debug::Highlight() << game.name << ": play" << Debug::Endl();
		game.state = Game::PLAYING;

		if( game.gameServer != nullptr ) game.gameServer->Start();
		else							 game.gameClient->Start();

		GameCamera& gameCamera = _world.GetSingleton<GameCamera>();
		scene.SetMainCamera( gameCamera.cameraHandle );		
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void  GameHolder::GameStop( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PLAYING || game.state == Game::PAUSED )
		{
			Debug::Highlight() << game.name << ": stopped" << Debug::Endl();
			game.state = Game::STOPPED;
			if( game.gameServer != nullptr ) game.gameServer->Stop();
			else							 game.gameClient->Stop();
		}
	}

	
	//================================================================================================================================
	//================================================================================================================================
	void  GameHolder::GamePause( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PLAYING )
		{
			Debug::Highlight() << game.name << ": paused" << Debug::Endl();
			game.state = Game::PAUSED;
		}
	}	

	//================================================================================================================================
	//================================================================================================================================
	void  GameHolder::GameResume( EcsWorld& _world )
	{
		Game& game = _world.GetSingleton<Game>();
		if( game.state == Game::PAUSED )
		{
			Debug::Highlight() << game.name << ": resumed" << Debug::Endl();
			game.state = Game::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameHolder::GameStep( EcsWorld& _world, float _delta )
	{
 		Game& game = _world.GetSingleton<Game>();
 		if( game.state == Game::PLAYING )
 		{
 			const float delta = ( game.state == Game::PLAYING ? _delta : 0.f );
 			if( game.gameServer != nullptr ) game.gameServer->Step( delta );
 			else							 game.gameClient->Step( delta );
 		}		
	}

	//================================================================================================================================
	// Updates the render world singleton component
	//================================================================================================================================
	void GameHolder::UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size )
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		renderWorld.targetSize = _size;

		// update render data
		_world.Run<S_UpdateRenderWorldModels>();
		_world.Run<S_UpdateRenderWorldUI>();
		_world.Run<S_UpdateRenderWorldPointLights>();
		_world.Run<S_UpdateRenderWorldDirectionalLights>();

		// particles mesh
		DrawMesh particlesDrawData;
		particlesDrawData.mesh = &renderWorld.particlesMesh;
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