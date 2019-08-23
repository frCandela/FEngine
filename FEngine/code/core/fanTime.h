#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Time
	{
	public:
		static float ElapsedSinceStartup() { return static_cast<float>(glfwGetTime()); }
		static double ElapsedSinceStartupDouble() { return glfwGetTime(); }

		static float GetFPS() { return ms_fps; }
		static void SetFPS(const float _fps) { ms_fps = _fps > minFps ? _fps : minFps; }
	
		static const float minFps;

		static std::string SecondsToString( const double _seconds );	// Returns a hours:minuts:seconds ex: 3661s = 01:01:01
	private:
		static float ms_fps;
	};
}