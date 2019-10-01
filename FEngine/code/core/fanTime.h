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

		float	GetDelta()	{ return m_delta;	 }
		void	SetDelta( const float _delta ) { m_delta = _delta; }
		void	UpdateFrameTime( const float _frameTime )	{ m_frameTime = _frameTime ;}
		float	GetFrameTime( )								{ return m_frameTime; }

		static std::string SecondsToString( const double _seconds );	// Returns a hours:minuts:seconds ex: 3783s = 01:02:03
	private:
		float m_delta = 1/60.f;
		float m_frameTime = 0.f;
	};
}