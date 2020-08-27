#include "network/singletons/fanTime.hpp"

#include <sstream>

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	float		Time::sRenderDelta            = 1.f / 60.f;
	float		Time::sLogicDelta             = 1.f / 60.f;
	float		Time::sPhysicsDelta           = 1.f / 60.f;
	uint32_t	Time::sFramesCounter           = 0;
	uint32_t	Time::sRealFramerateLastSecond = 0;
	double		Time::sLastLogFrameTime      = 0.f;

	//========================================================================================================
	//========================================================================================================
	void Time::SetInfo( EcsSingletonInfo& _info )
	{ 
		_info.mIcon  = ImGui::Time16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &Time::OnGui;
		_info.mName  = "time";
	}

	//========================================================================================================
	//========================================================================================================
	void Time::Init( EcsWorld& /*_world*/, EcsSingleton& _component ){
		Time& gameTime = static_cast<Time&>( _component );
		gameTime.mFrameIndex         = 0;
		gameTime.mFrameStart         = 0;
		gameTime.mLogicDelta         = 1.f / 60.f;
		gameTime.mTimeScaleDelta     = 0.f;
		// timeScaleIncrement -> it takes 20 frames to time scale one frame ( 5% faster/slower )
		gameTime.mTimeScaleIncrement = gameTime.mLogicDelta / 20.f;
		gameTime.mLastLogicTime      = 0.;
	}

	//========================================================================================================
	// Counts the frames during the last second to calculate the framerate
	//========================================================================================================
	void Time::RegisterFrameDrawn()
	{
		++sFramesCounter;

		const double time = ElapsedSinceStartup();
		if( time - sLastLogFrameTime > 1.f )
		{
            sRealFramerateLastSecond = sFramesCounter;
            sFramesCounter           = 0;
            sLastLogFrameTime        = time;
		}
	}

	//========================================================================================================
	// Returns a hours:minuts:seconds ex: 3783s = 01:02:03
	//========================================================================================================
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

	//========================================================================================================
	// Client must adjust it frame index to be in sync with the server
	// if the delta is small enough, use timescale
	// if it's too big, offsets the frame index directly
	//========================================================================================================
	void Time::OnShiftFrameIndex( const int _framesDelta )
	{
		if( std::abs( _framesDelta ) > Time::sMaxFrameDeltaBeforeShift )
		{
            mFrameIndex += _framesDelta;
		}
		else
		{
            mTimeScaleDelta = _framesDelta * mLogicDelta;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Time::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		Time& gameTime = static_cast<Time&>( _component );
		ImGui::Text( "frame index:          %d", gameTime.mFrameIndex );
		ImGui::Text( "frame start:          %d", gameTime.mFrameStart );
		ImGui::Text( "logic delta:          %.3f", gameTime.mLogicDelta );
		ImGui::Text( "time scale increment: %.3f", gameTime.mTimeScaleIncrement );
		ImGui::DragFloat( "timescale", &gameTime.mTimeScaleDelta, 0.1f );
	}
}