#include "game/components/fanAnimScale.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AnimScale::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &AnimScale::Load;
        _info.save = &AnimScale::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AnimScale::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        AnimScale& animScale = static_cast<AnimScale&>( _component );
        animScale.mSourceScale = Vector3::sOne;
        animScale.mTargetScale = Vector3::sZero;
        animScale.mDuration = 1;
        animScale.mTimeAccumulator = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AnimScale::Save( const EcsComponent& _component, Json& _json )
    {
        const AnimScale& animScale = static_cast<const AnimScale&>( _component );
        Serializable::SaveVec3( _json, "source_scale", animScale.mSourceScale );
        Serializable::SaveVec3( _json, "target_scale", animScale.mTargetScale );
        Serializable::SaveFixed( _json, "duration", animScale.mDuration );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void AnimScale::Load( EcsComponent& _component, const Json& _json )
    {
        AnimScale& animScale = static_cast<AnimScale&>( _component );
        Serializable::LoadVec3( _json, "source_scale", animScale.mSourceScale );
        Serializable::LoadVec3( _json, "target_scale", animScale.mTargetScale );
        Serializable::LoadFixed( _json, "duration", animScale.mDuration );
    }
}