#pragma once

#include <string>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/vec2.hpp"
WARNINGS_POP()

namespace fan
{
	//================================================================================================================================
	// the launch parameters of the engine
	//================================================================================================================================
	struct LaunchSettings
	{
		enum class Mode { EditorClient, EditorServer, EditorClientServer, Client, Server };

		std::string windowName = "FEngine";				// sets the name of the application window
		std::string	loadScene = "";						// loads a scene at startup
		bool		autoPlay = false;					// auto play the scene loaded at startup
		bool		enableLivepp = false;				// enables Live++ hot reload
		bool		mainLoopSleep = false;				// enables sleeping instead of busy waiting in the main loop ( causes frame drops )
		Mode		launchMode = Mode::EditorClientServer;	// launch as server and client with the editor 
		glm::ivec2  window_position = { -1,-1 };
		glm::ivec2  window_size = { -1,-1 };
	};
}