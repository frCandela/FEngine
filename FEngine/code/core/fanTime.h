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
		static float	GetDelta()	{ return Get().m_delta;	 }
		static float	GetFPS()	{ return 1.f / Get().m_delta; }
		static void		SetFPS(const float _fps) { Get().m_delta =  1.f / std::clamp(_fps, Get().m_minFps, Get().m_maxFps);	}

		static std::string SecondsToString( const double _seconds );	// Returns a hours:minuts:seconds ex: 3661s = 01:01:01
	private:
		float m_minFps = 10.f;
		float m_maxFps = 144.f;
		float m_delta = 1/60.f;
	};
}