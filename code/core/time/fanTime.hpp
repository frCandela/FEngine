#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/fanSingleton.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Time : public Singleton<Time>
	{
		friend class Singleton<Time>;

	public:
		static float ElapsedSinceStartup() { return static_cast< float >( glfwGetTime() ); }
		static double ElapsedSinceStartupDouble() { return glfwGetTime(); }

		float	GetLogicDelta()	const { return m_logicDelta; }
		void	SetLogicDelta( const float _delta ) { m_logicDelta = _delta; }
		float	GetRenderDelta()const { return m_renderDelta; }
		void	SetRenderDelta( const float _delta ) { m_renderDelta = _delta; }
		float	GetPhysicsDelta()const { return m_physicsDelta; }
		void	SetPhysicsDelta( const float _delta ) { m_physicsDelta = _delta; }

		void     RegisterFrameDrawn();
		uint32_t GetRealFramerate() { return m_realFramerateLastSecond; }

		static std::string SecondsToString( const double _seconds );	// Returns a hours:minuts:seconds ex: 3783s = 01:02:03
	protected:
		Time();

	private:
		float m_renderDelta;
		float m_logicDelta;
		float m_physicsDelta;

		uint32_t m_framesCounter = 0;
		uint32_t m_realFramerateLastSecond = 0;
		float m_lastLogFrameTime = 0.f;
	};
}