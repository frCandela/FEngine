#include "network/singletons/fanTime.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Time::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = Time::Save;
        _info.load = Time::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Time::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        Time& time = static_cast<Time&>( _component );
        time.mFrameIndex  = 0;
        time.mLogicDelta  = FIXED( 1 ) / 120;
        time.mRenderDelta = FIXED( 1 ) / 144;

        time.mLastLogicTime    = ElapsedSinceStartup();
        time.mLastRenderTime   = ElapsedSinceStartup();
        time.mAverageFrameTime = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Time::Save( const EcsSingleton& _component, Json& _json )
    {
        const Time& time = static_cast<const Time&>( _component );
        Serializable::SaveFixed( _json, "logic_delta", time.mLogicDelta );
        Serializable::SaveFixed( _json, "render_delta", time.mRenderDelta );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Time::Load( EcsSingleton& _component, const Json& _json )
    {
        Time& time = static_cast<Time&>( _component );
        Serializable::LoadFixed( _json, "logic_delta", time.mLogicDelta );
        Serializable::LoadFixed( _json, "render_delta", time.mRenderDelta );
    }

    //==================================================================================================================================================================================================
    // Counts the frames during the last second to calculate the framerate
    //==================================================================================================================================================================================================
    void Time::RegisterFrameDrawn( Time& _time, double _frameTime )
    {
        static const double ratio = 0.95f;
        _time.mAverageFrameTime = ratio * _time.mAverageFrameTime + ( 1 - ratio ) * _frameTime;
    }
}