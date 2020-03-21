#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class Game;
	class GameWindow;
	class Renderer;

	//================================================================================================================================
	//================================================================================================================================
	class EditorGameWindowCallbacks
	{
	public:
		EditorGameWindowCallbacks( Game& _game  );
		void ConnectCallbacks( GameWindow& _gameWindow, Renderer& _renderer );

		void OnGamePlay();
		void OnGamePause();
		void OnGameResume();
		void OnGameStop();
		void OnGameStep();

	private:
		Game& m_game;
	};
}
