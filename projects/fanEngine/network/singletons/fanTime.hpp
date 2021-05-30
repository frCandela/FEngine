#pragma once

#include <glfw/glfw3.h>
#include "core/ecs/fanEcsSingleton.hpp"
#include "network/fanNetConfig.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Time : public EcsSingleton
    {
    ECS_SINGLETON( Time )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        Fixed mLogicDelta;     // duration of a logic frame in seconds
        Fixed mRenderDelta;    // duration of a physics step in seconds

        FrameIndex mFrameIndex;         // the index of the current logic frame
        double     mLastLogicTime;      // last time the logic was run
        double     mLastRenderTime;     // last time the frame was drawn
        double     mAverageFrameTime;   // for calculating framerate

        static double ElapsedSinceStartup() { return glfwGetTime(); }
        static void RegisterFrameDrawn( Time& _time, double _frameTime );
    };
}