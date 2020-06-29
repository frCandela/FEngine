#pragma once

namespace fan
{
	//================================================================================================================================
	// the launch parameters of the engine
	//================================================================================================================================
	struct LaunchSettings
	{
		enum Mode { Client, Server, ClientServer };

		std::string windowName = "FEngine";		// sets the name of the application window
		std::string	loadScene = "";				// loads a scene at startup
		bool		autoPlay = false;			// auto play the scene loaded at startup
		bool		enableLivepp = false;		// enables Live++ hot reload
		bool		mainLoopSleep = false;		// enables sleeping instead of busy waiting in the main loop ( causes frame drops )
		Mode		launchMode = ClientServer;	// launch as server or client
		glm::ivec2  window_position = { -1,-1 };	// forces the position of the window
		glm::ivec2  window_size = { -1,-1 };	// forces the size of the window
	};
}