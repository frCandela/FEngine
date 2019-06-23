#pragma once

class Time
{
public:
	static float ElapsedSinceStartup() { return static_cast<float>(glfwGetTime()); }


	static float GetFPS() { return ms_fps; }
	static void SetFPS(const float _fps) { ms_fps = _fps > minFps ? _fps : minFps; }
	
	static const float minFps;

private:
	static float ms_fps;
};