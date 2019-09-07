#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Time : public Singleton<Time>
	{
	public:
		static float ElapsedSinceStartup() { return static_cast<float>(glfwGetTime()); }
		static double ElapsedSinceStartupDouble() { return glfwGetTime(); }

		static float	GetMinFPS() { return Get().m_minFps; }
		static float	GetMaxFPS() { return Get().m_maxFps; }
		static float	GetFPS() { return Get().m_fps; }
		static void		SetFPS(const float _fps) { Get().m_fps = _fps > Get().m_minFps ? _fps : Get().m_minFps; }

		static std::string SecondsToString( const double _seconds );	// Returns a hours:minuts:seconds ex: 3661s = 01:01:01
	private:
		float m_minFps = 10.f;
		float m_maxFps = 144.f;
		float m_fps = 60.f;
	};
}