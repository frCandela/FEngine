#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Time : public Singleton<Time>
	{
		friend class Singleton<Time>;

	public:
		static float ElapsedSinceStartup() { return static_cast<float>(glfwGetTime()); }
		static double ElapsedSinceStartupDouble() { return glfwGetTime(); }

		float	GetMinFPS() { return m_minFps; }
		float	GetMaxFPS() { return m_maxFps; }
		float	GetDelta()	{ return m_delta;	 }
		float	GetFPS()	{ return 1.f / m_delta; }
		void	SetFPS(const float _fps) { m_delta =  1.f / std::clamp(_fps, m_minFps, m_maxFps);	}
		bool	GetCapFPS() { return m_capFPS ;}
		void	SetCapFPS( const bool _capFPS ) { m_capFPS = _capFPS; }

		static std::string SecondsToString( const double _seconds );	// Returns a hours:minuts:seconds ex: 3661s = 01:01:01
	private:
		float m_minFps = 1.f;
		float m_maxFps = 144.f;
		float m_delta = 1/60.f;
		bool  m_capFPS = true;
	};
}