#include "engine/components/fanFxScale.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxScale::SetInfo( EcsComponentInfo& _info )
    {
        _info.save = &FxScale::Save;
        _info.load = &FxScale::Load;
    }

    //========================================================================================================
    //========================================================================================================
    void FxScale::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        FxScale& scale = static_cast<FxScale&>( _component );
        scale.mScale = Vector3::sOne;
    }

    //========================================================================================================
    //========================================================================================================
    void FxScale::Save( const EcsComponent& _component, Json& _json )
    {
        const FxScale& scale = static_cast<const FxScale&>( _component );
        Serializable::SaveVec3( _json, "scale", scale.mScale );
    }

    //========================================================================================================
    //========================================================================================================
    void FxScale::Load( EcsComponent& _component, const Json& _json )
    {
        FxScale& scale = static_cast<FxScale&>( _component );
        Serializable::LoadVec3( _json, "scale", scale.mScale );
    }
}