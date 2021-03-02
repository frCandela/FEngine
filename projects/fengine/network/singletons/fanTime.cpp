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
	void Time::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
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
}