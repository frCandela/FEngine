#pragma once

#include <string>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/vec2.hpp"
WARNINGS_POP()

namespace fan
{
	//========================================================================================================
	// the launch parameters of the engine
	//========================================================================================================
	struct LaunchSettings
	{
		std::string mWindowName            = "FEngine"; // sets the name of the application window
		std::string mLoadScene             = "";	    // loads a scene at startup
		std::string mIconPath              = "";        // sets application icon
		bool        mAutoPlay              = false;	    // auto play the scene loaded at startup
		bool        mMainLoopSleep         = false;	    // enables sleeping instead of busy waiting
		bool        mLaunchEditor          = true;      // launch in an editor holder
        bool        mForceWindowDimensions = false;     // window position/size were set from the command line
        glm::ivec2  mWindow_position       = { -1, -1 };
        glm::ivec2  mWindow_size           = { -1, -1 };

#ifdef NDEBUG
        bool mEnableLivepp = false;	    // disables Live++ hot reload
#else
        bool mEnableLivepp = true;	    // enables Live++ hot reload
#endif // !NDEBUG
	};
}