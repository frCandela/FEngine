#include "network/singletons/fanTime.hpp"

#include <sstream>

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	float		Time::s_renderDelta = 1.f / 60.f;
	float		Time::s_logicDelta = 1.f / 60.f;
	float		Time::s_physicsDelta = 1.f / 60.f;
	uint32_t	Time::s_framesCounter = 0;
	uint32_t	Time::s_realFramerateLastSecond = 0;
	double		Time::s_lastLogFrameTime = 0.f;

	//================================================================================================================================
	//================================================================================================================================
	void Time::SetInfo( EcsSingletonInfo& _info )
	{ 
		_info.icon = ImGui::NONE;
		_info.onGui = &Time::OnGui;
		_info.name = "game time";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Time::Init( EcsWorld& /*_world*/, EcsSingleton& _component ){
		Time& gameTime = static_cast<Time&>( _component );
		gameTime.frameIndex = 0;
		gameTime.frameStart = 0;
		gameTime.logicDelta = 1.f / 60.f;
		gameTime.timeScaleDelta = 0.f;
		gameTime.timeScaleIncrement = gameTime.logicDelta / 20.f; // it takes 20 frames to time scale one frame ( 5% faster/slower )
		gameTime.lastLogicTime = 0.;
	}

	//================================================================================================================================
	// Counts the frames during the last second to calculate the framerate
	//================================================================================================================================
	void Time::RegisterFrameDrawn()
	{
		++s_framesCounter;

		const double time = ElapsedSinceStartup();
		if( time - s_lastLogFrameTime > 1.f )
		{
			s_realFramerateLastSecond = s_framesCounter;
			s_framesCounter = 0;
			s_lastLogFrameTime = time;
		}
	}

	//================================================================================================================================
	// Returns a hours:minuts:seconds ex: 3783s = 01:02:03
	//================================================================================================================================
	std::string Time::SecondsToString( const double _seconds )
	{
		std::stringstream ss;
		unsigned remainingSeconds = static_cast<unsigned>( _seconds );

		const unsigned hours = remainingSeconds / 3600;
		remainingSeconds -= hours * 3600;
		ss << '[';
		if( hours < 10 )
		{
			ss << '0';
		}
		ss << hours << ':';

		const unsigned minuts = remainingSeconds / 60;
		remainingSeconds -= minuts * 60;
		if( minuts < 10 )
		{
			ss << 0;
		}
		ss << minuts << ':';

		const unsigned seconds = remainingSeconds;
		if( seconds < 10 )
		{
			ss << '0';
		}
		ss << seconds << ']';
		return ss.str();
	}

	//================================================================================================================================
	// Client must adjust it frame index to be in sync with the server
	// if the delta is small enough, use timescale
	// if it's too big, offsets the frame index directly
	//================================================================================================================================
	void Time::OnShiftFrameIndex( const int _framesDelta )
	{
		if( std::abs( _framesDelta ) > Time::s_maxFrameDeltaBeforeShift )
		{
			frameIndex += _framesDelta;
		}
		else
		{
			timeScaleDelta = _framesDelta * logicDelta;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Time::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		Time& gameTime = static_cast<Time&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "frame index:          %d", gameTime.frameIndex );
			ImGui::Text( "frame start:          %d", gameTime.frameStart );
			ImGui::Text( "logic delta:          %.3f", gameTime.logicDelta );
			ImGui::Text( "time scale increment: %.3f", gameTime.timeScaleIncrement );
			ImGui::DragFloat( "timescale", &gameTime.timeScaleDelta, 0.1f );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}