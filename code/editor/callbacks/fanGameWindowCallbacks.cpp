#include "editor/callbacks/fanGameWindowCallbacks.hpp"
#include "editor/windows/fanGameWindow.hpp"
#include "core/time/fanTime.hpp"
#include "render/fanRenderer.hpp"
#include "game/fanGame.hpp"

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
	void EditorGameWindowCallbacks::OnGamePlay() { m_game.Play(); }
	void EditorGameWindowCallbacks::OnGamePause() { m_game.Pause(); }
	void EditorGameWindowCallbacks::OnGameResume() { m_game.Resume();}
	void EditorGameWindowCallbacks::OnGameStop() { m_game.Stop();}
	void EditorGameWindowCallbacks::OnGameStep() { m_game.Step( Time::Get().GetLogicDelta() );}
}