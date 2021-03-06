#include "engine/components/fanScale.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scale::SetInfo( EcsComponentInfo& _info )
    {
        _info.save = &Scale::Save;
        _info.load = &Scale::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scale::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Scale& scale = static_cast<Scale&>( _component );
        scale.mScale = Vector3::sOne;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scale::Save( const EcsComponent& _component, Json& _json )
    {
        const Scale& scale = static_cast<const Scale&>( _component );
        Serializable::SaveVec3( _json, "scale", scale.mScale );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scale::Load( EcsComponent& _component, const Json& _json )
    {
        Scale& scale = static_cast<Scale&>( _component );
        Serializable::LoadVec3( _json, "scale", scale.mScale );
    }
}