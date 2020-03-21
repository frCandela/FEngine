#include "editor/callbacks/fanGameWindowCallbacks.hpp"

#include "editor/windows/fanGameWindow.hpp"
#include "core/time/fanTime.hpp"
#include "render/fanRenderer.hpp"
#include "game/fanGame.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/singletonComponents/fanScene.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EditorGameWindowCallbacks::EditorGameWindowCallbacks( Game& _game )
		: m_game( _game )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGameWindowCallbacks::ConnectCallbacks( GameWindow& _gameWindow, Renderer& _renderer )
	{
		_gameWindow.onSizeChanged.Connect( &Renderer::ResizeGame, &_renderer );

		_gameWindow.onPlay.Connect( &EditorGameWindowCallbacks::OnGamePlay, this );
		_gameWindow.onPause.Connect( &EditorGameWindowCallbacks::OnGamePause, this );
		_gameWindow.onResume.Connect( &EditorGameWindowCallbacks::OnGameResume, this );
		_gameWindow.onStop.Connect( &EditorGameWindowCallbacks::OnGameStop, this );
		_gameWindow.onStep.Connect( &EditorGameWindowCallbacks::OnGameStep, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGameWindowCallbacks::OnGamePlay() { 
		if( m_game.scene.path.empty() )
		{
			Debug::Warning() << "please save the scene before playing" << Debug::Endl();
			return;
		}


		assert( m_game.state == Game::STOPPED );
		m_game.scene.Save();
		m_game.Play(); 
	}
	void EditorGameWindowCallbacks::OnGamePause() { m_game.Pause(); }
	void EditorGameWindowCallbacks::OnGameResume() { m_game.Resume();}
	void EditorGameWindowCallbacks::OnGameStep() { m_game.Step( Time::Get().GetLogicDelta() );}
	void EditorGameWindowCallbacks::OnGameStop()
	{
		// Saves the camera position for restoring it later
		const EntityID oldCameraID = m_game.world.GetEntityID( m_game.scene.mainCamera->handle );
		const btTransform oldCameraTransform = m_game.world.GetComponent<Transform>( oldCameraID ).transform;

		m_game.Stop(); 
		m_game.scene.LoadFrom( m_game.scene.path ); // reload

		const EntityID newCameraID = m_game.world.GetEntityID( m_game.scene.mainCamera->handle );
		m_game.world.GetComponent<Transform>( newCameraID ).transform = oldCameraTransform;
	}
}