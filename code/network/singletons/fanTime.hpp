#pragma once

#include "glfw/glfw3.h"
#include "ecs/fanEcsSingleton.hpp"
#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct Time : public EcsSingleton
	{
		ECS_SINGLETON( Time )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		FrameIndex mFrameIndex;			// the index of the current frame
		FrameIndex mFrameStart;			// the index of the first frame of the game
		float      mLogicDelta;			// time between two frames in seconds
		float      mTimeScaleDelta;		// (seconds) accelerate, decelerates the logic frame rate to resync frame index with server
		float      mTimeScaleIncrement; // the maximum amount that can be added to each frame
		double     mLastLogicTime;		// last time the logic step was called

		static const int sMaxFrameDeltaBeforeShift = 20; // if the server/client frame delta > this, shift frameIndex. Otherwise use timescale
		static float     sRenderDelta;
		static float     sLogicDelta;
		static float     sPhysicsDelta;
		static uint32_t  sFramesCounter;
		static uint32_t  sRealFramerateLastSecond;
		static double    sLastLogFrameTime;

		static double		ElapsedSinceStartup() { return glfwGetTime(); }
		static void			RegisterFrameDrawn();
		static std::string	SecondsToString( const double _seconds );	

		void OnShiftFrameIndex( const int _framesDelta );
	};
}