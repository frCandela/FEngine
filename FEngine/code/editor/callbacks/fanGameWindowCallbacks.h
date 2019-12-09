#pragma once

namespace fan
{
	class Scene;
	class GameWindow;
	class Renderer;

	//================================================================================================================================
	//================================================================================================================================
	class EditorGameWindowCallbacks
	{
	public:
		EditorGameWindowCallbacks( Scene& _clientScene, Scene& _serverScene );
		void SetupGameWindow( GameWindow& _gameWindow, Renderer& _renderer );

		void OnGamePlay();
		void OnGamePause();
		void OnGameResume();
		void OnGameStop();
		void OnGameStep();

	private:
		Scene&	m_clientScene;
		Scene&	m_serverScene;
	};
}
