#include "fanGlobalIncludes.h"
#include "editor/callbacks/fanGameWindowCallbacks.h"

#include "scene/fanScene.h"
#include "core/time/fanTime.h"
#include "editor/windows/fanGameWindow.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EditorGameWindowCallbacks::EditorGameWindowCallbacks( Scene& _clientScene, Scene& _serverScene )	
		: m_clientScene(_clientScene)
		, m_serverScene(_serverScene)
	{}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGameWindowCallbacks::ConnectCallbacks( GameWindow& _gameWindow, Renderer& _renderer )
	{
		_gameWindow.onSizeChanged.Connect( &Renderer::ResizeGame, &_renderer );

		_gameWindow.onPlay  .Connect( &EditorGameWindowCallbacks::OnGamePlay, this );
		_gameWindow.onPause .Connect( &EditorGameWindowCallbacks::OnGamePause, this );
		_gameWindow.onResume.Connect( &EditorGameWindowCallbacks::OnGameResume, this );
		_gameWindow.onStop  .Connect( &EditorGameWindowCallbacks::OnGameStop, this );
		_gameWindow.onStep  .Connect( &EditorGameWindowCallbacks::OnGameStep, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorGameWindowCallbacks::OnGamePlay()  { m_clientScene.Play();									m_serverScene.Play(); }
	void EditorGameWindowCallbacks::OnGamePause() { m_clientScene.Pause();								m_serverScene.Pause();}
	void EditorGameWindowCallbacks::OnGameResume(){ m_clientScene.Resume();								m_serverScene.Resume();}
	void EditorGameWindowCallbacks::OnGameStop()  { m_clientScene.Stop();									m_serverScene.Stop();}
	void EditorGameWindowCallbacks::OnGameStep()  { m_clientScene.Step( Time::Get().GetLogicDelta() );	m_serverScene.Step( Time::Get().GetLogicDelta() );}
}