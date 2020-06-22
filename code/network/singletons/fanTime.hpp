#pragma once

#include "glfw/glfw3.h"
#include "ecs/fanEcsSingleton.hpp"
#include "network/fanNetConfig.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	struct GameClient;
	struct GameServer;

	//================================================================================================================================
	// Contains game data & a reference to the game client or server depending on which is used
	// allows saving properties of the game into the scene & edition of parameters from the editor
	// @todo, put time related stuff in a singleton in the scene lib
	//================================================================================================================================	
	struct Time : public EcsSingleton
	{
		ECS_SINGLETON( Time )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		FrameIndex	frameIndex;			// the index of the current time
		FrameIndex	frameStart;			// the index of the first frame of the game
		float		logicDelta;			// time between two frames in seconds
		float		timeScaleDelta;		// (seconds) accelerate, decelerates the logic frame rate to resync frame index with server
		float		timeScaleIncrement; // the maximum amount that can be added to each frame
		
		static const int	s_maxFrameDeltaBeforeShift = 20; // if the server/client frame delta > this, shift frameIndex. Otherwise use timescale
		static float		s_renderDelta;
		static float		s_logicDelta;
		static float		s_physicsDelta;
		static uint32_t		s_framesCounter;
		static uint32_t		s_realFramerateLastSecond;
		static double		s_lastLogFrameTime;

		static double		ElapsedSinceStartup() { return glfwGetTime(); }
		static void			RegisterFrameDrawn();
		static std::string	SecondsToString( const double _seconds );	

		void OnShiftFrameIndex( const int _framesDelta );
	};
}